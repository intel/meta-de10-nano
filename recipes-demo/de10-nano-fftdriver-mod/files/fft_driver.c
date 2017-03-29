/*
 * Copyright (C) 2017 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>

#include "fft_driver.h"

/* globals */
static struct semaphore g_dev_probe_sem;
static int g_platform_probe_flag;

/* structures */
struct fft_samples_struct {
	short i;
	short r;
};

struct raw_samples_struct {
	long r;
	long i;
};

/* fft_dev structure and storage allocation */
struct fft_dev {
	struct semaphore sem;
	uint32_t phys_fft_st_adapter;
	uint32_t size_fft_st_adapter;
	void *ioremap_fft_st_adapter;
	uint32_t phys_fft_data_ram;
	uint32_t size_fft_data_ram;
	void *ioremap_fft_data_ram;
	uint32_t phys_sgdma_from_fft_descriptor;
	uint32_t size_sgdma_from_fft_descriptor;
	void *ioremap_sgdma_from_fft_descriptor;
	uint32_t phys_sgdma_from_fft_csr;
	uint32_t size_sgdma_from_fft_csr;
	void *ioremap_sgdma_from_fft_csr;
	uint32_t phys_sgdma_to_fft_descriptor;
	uint32_t size_sgdma_to_fft_descriptor;
	void *ioremap_sgdma_to_fft_descriptor;
	uint32_t phys_sgdma_to_fft_csr;
	uint32_t size_sgdma_to_fft_csr;
	void *ioremap_sgdma_to_fft_csr;
	uint32_t phys_sgdma_from_ram_descriptor;
	uint32_t size_sgdma_from_ram_descriptor;
	void *ioremap_sgdma_from_ram_descriptor;
	uint32_t phys_sgdma_from_ram_csr;
	uint32_t size_sgdma_from_ram_csr;
	void *ioremap_sgdma_from_ram_csr;
	int fft_open_for_read;
	int fft_open_for_write;
	int fft256_open_for_read;
	int fft256_open_for_write;
	int fftdma_open_for_read;
	int fftdma_open_for_write;
	int fft256dma_open_for_read;
	int fft256dma_open_for_write;
	int fft256stream_open_for_read;
	int fft256stream_open_for_write;
	int raw256stream_open_for_read;
	int raw256stream_open_for_write;
	uint32_t fft256_buffer_read_index;
	uint32_t fft256_buffer_write_index;
	uint32_t fft256_buffer_inflight_count;
	void *dma_buffer;
	dma_addr_t dma_handle;
};

static struct fft_dev the_fft_dev = {
	/*
	   .sem = initialize this at runtime before it is needed
	 */
};

/* misc device - raw256stream_dev */
static ssize_t raw256stream_dev_write(struct file *fp,
				      const char __user *user_buffer,
				      size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if (count > (64 * 1024)) {
		up(&dev->sem);
		return -EINVAL;
	}

	/* move the input data from the user buffer into the fft data buffer */
	if (copy_from_user
	    (dev->ioremap_fft_data_ram + DMA_DATA_OFFSET, user_buffer, count)) {
		up(&dev->sem);
		pr_info("raw256stream_dev_write copy_from_user exit\n");
		return -EFAULT;
	}

	up(&dev->sem);
	return count;
}

static ssize_t raw256stream_dev_read(struct file *fp, char __user *user_buffer,
				     size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	uint32_t dma_status;
	uint32_t dma_desc_fill_level;
	int this_count = 256;
	int write_block_count = 0;
	int read_block_count = 0;
	int read_count = 0;
	int next_fft256_buffer_offset;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/* validate count request */
	if (count & ((this_count * 8) - 1)) {
		up(&dev->sem);
		return -EINVAL;
	}

	if (count > 0)
		read_block_count = count / (this_count * 8);
	else
		read_block_count = 0;

	write_block_count = read_block_count;

	while ((write_block_count > 0) || (read_block_count > 0)) {
		while (write_block_count > 0) {
			if (dev->fft256_buffer_inflight_count >= 32)
				break;

			next_fft256_buffer_offset =
			    dev->fft256_buffer_write_index * this_count * 8;

			/* start the read from RAM DMA */
			iowrite32(DMA_RAM_DATA_ADDR,
				  dev->ioremap_sgdma_from_ram_descriptor +
				  DESC_READ_ADDRESS_REG);
			iowrite32(dev->dma_handle + next_fft256_buffer_offset,
				  dev->ioremap_sgdma_from_ram_descriptor +
				  DESC_WRITE_ADDRESS_REG);
			iowrite32(this_count * 8,
				  dev->ioremap_sgdma_from_ram_descriptor +
				  DESC_LENGTH_REG);
			iowrite32(START_DMA_FROM_FFT_MASK,
				  dev->ioremap_sgdma_from_ram_descriptor +
				  DESC_CONTROL_REG);

			/* maintain counters and pointers */
			write_block_count--;
			dev->fft256_buffer_inflight_count++;
			dev->fft256_buffer_write_index++;
			if (dev->fft256_buffer_write_index >= 32)
				dev->fft256_buffer_write_index = 0;
		}

		while (read_block_count > 0) {
			if (dev->fft256_buffer_inflight_count == 0)
				break;

			if (dev->fft256_buffer_inflight_count >= 2) {
				/* wait for at least two descriptors to be used
				*/
				do {
					dma_desc_fill_level =
					    ioread32(dev->
						     ioremap_sgdma_from_ram_csr
						     +
						 CSR_DESCRIPTOR_FILL_LEVEL_REG);
					dma_desc_fill_level &=
					CSR_WRITE_FILL_LEVEL_MASK;
					dma_desc_fill_level >>=
				      CSR_WRITE_FILL_LEVEL_OFFSET;
				} while (dma_desc_fill_level >
					 (dev->fft256_buffer_inflight_count -
					  2));
			} else {
				/* wait for all descriptors to be used */
				do {
					dma_desc_fill_level =
					    ioread32(dev->
						     ioremap_sgdma_from_ram_csr
						     +
						 CSR_DESCRIPTOR_FILL_LEVEL_REG);
					dma_desc_fill_level &=
					CSR_WRITE_FILL_LEVEL_MASK;
					dma_desc_fill_level >>=
				      CSR_WRITE_FILL_LEVEL_OFFSET;
				} while (dma_desc_fill_level > 0);

				/* wait for read from FFT DMA to complete */
				do {
					dma_status =
					    ioread32(dev->
						     ioremap_sgdma_from_ram_csr
						     + CSR_STATUS_REG);
				} while ((dma_status &
					  CSR_BUSY_MASK) != 0);
			}

			/* move the results buffer into the user buffer */
			next_fft256_buffer_offset =
			    dev->fft256_buffer_read_index * this_count * 8;
			if (copy_to_user
			    (user_buffer,
			     dev->dma_buffer + next_fft256_buffer_offset,
			     this_count * 8)) {
				up(&dev->sem);
				pr_info
				  ("raw256stream_dev_read copy_to_user exit\n");
				return -EFAULT;
			}

			/* maintain counters and pointers */
			user_buffer += this_count * 8;
			read_count += this_count * 8;
			read_block_count--;
			dev->fft256_buffer_inflight_count--;
			dev->fft256_buffer_read_index++;
			if (dev->fft256_buffer_read_index >= 32)
				dev->fft256_buffer_read_index = 0;
		}
	}

	up(&dev->sem);
	return read_count;
}

static int raw256stream_dev_open(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = &the_fft_dev;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = -EBUSY;
	if ((dev->fft256stream_open_for_read != 0) ||
	    (dev->fft256stream_open_for_write != 0) ||
	    (dev->fft_open_for_read != 0) ||
	    (dev->fft_open_for_write != 0) ||
	    (dev->fftdma_open_for_read != 0) ||
	    (dev->fftdma_open_for_write != 0) ||
	    (dev->fft256_open_for_read != 0) ||
	    (dev->fft256_open_for_write != 0))
		goto do_exit;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		if (dev->raw256stream_open_for_read != 0)
			goto do_exit;
		dev->raw256stream_open_for_read = 1;
		break;
	case (O_WRONLY):
		if (dev->raw256stream_open_for_write != 0)
			goto do_exit;
		dev->raw256stream_open_for_write = 1;
		break;
	case (O_RDWR):
		if (dev->raw256stream_open_for_read != 0)
			goto do_exit;
		if (dev->raw256stream_open_for_write != 0)
			goto do_exit;
		dev->raw256stream_open_for_read = 1;
		dev->raw256stream_open_for_write = 1;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

	ret_val = 0;
	fp->private_data = dev;

do_exit:
	up(&dev->sem);
	return ret_val;
}

static int raw256stream_dev_release(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = fp->private_data;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = 0;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		dev->raw256stream_open_for_read = 0;
		break;
	case (O_WRONLY):
		dev->raw256stream_open_for_write = 0;
		break;
	case (O_RDWR):
		dev->raw256stream_open_for_read = 0;
		dev->raw256stream_open_for_write = 0;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

do_exit:
	up(&dev->sem);
	return ret_val;
}

static const struct file_operations raw256stream_dev_fops = {
	.owner = THIS_MODULE,
	.open = raw256stream_dev_open,
	.release = raw256stream_dev_release,
	.read = raw256stream_dev_read,
	.write = raw256stream_dev_write,
};

static struct miscdevice raw256stream_dev_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "raw256stream",
	.fops = &raw256stream_dev_fops,
};

/* misc device - fft256stream_dev */
static ssize_t fft256stream_dev_write(struct file *fp,
				      const char __user *user_buffer,
				      size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if (count > (64 * 1024)) {
		up(&dev->sem);
		return -EINVAL;
	}

	/* move the input data from the user buffer into the fft data buffer */
	if (copy_from_user
	    (dev->ioremap_fft_data_ram + DMA_DATA_OFFSET, user_buffer, count)) {
		up(&dev->sem);
		pr_info("fft256stream_dev_write copy_from_user exit\n");
		return -EFAULT;
	}

	up(&dev->sem);
	return count;
}

static ssize_t fft256stream_dev_read(struct file *fp, char __user *user_buffer,
				     size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	uint32_t dma_status;
	uint32_t dma_desc_fill_level;
	int this_count = 256;
	int write_block_count = 0;
	int read_block_count = 0;
	int read_count = 0;
	int next_fft256_buffer_offset;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/* validate count request */
	if (count & ((this_count * 8) - 1)) {
		up(&dev->sem);
		return -EINVAL;
	}

	if (count > 0)
		read_block_count = count / (this_count * 8);
	else
		read_block_count = 0;

	write_block_count = read_block_count;

	/* set the FFT length */
	iowrite32(this_count, dev->ioremap_fft_st_adapter);

	while ((write_block_count > 0) || (read_block_count > 0)) {
		while (write_block_count > 0) {
			if (dev->fft256_buffer_inflight_count >= 32)
				break;

			next_fft256_buffer_offset =
			    dev->fft256_buffer_write_index * this_count * 8;

			/* start the read from FFT DMA */
			iowrite32(0x0,
				  dev->ioremap_sgdma_from_fft_descriptor +
				  DESC_READ_ADDRESS_REG);
			iowrite32(dev->dma_handle + next_fft256_buffer_offset,
				  dev->ioremap_sgdma_from_fft_descriptor +
				  DESC_WRITE_ADDRESS_REG);
			iowrite32(this_count * 8,
				  dev->ioremap_sgdma_from_fft_descriptor +
				  DESC_LENGTH_REG);
			iowrite32(START_DMA_FROM_FFT_MASK,
				  dev->ioremap_sgdma_from_fft_descriptor +
				  DESC_CONTROL_REG);

			/* start the write to FFT DMA */
			iowrite32(DMA_RAM_DATA_ADDR,
				  dev->ioremap_sgdma_to_fft_descriptor +
				  DESC_READ_ADDRESS_REG);
			iowrite32(0x0,
				  dev->ioremap_sgdma_to_fft_descriptor +
				  DESC_WRITE_ADDRESS_REG);
			iowrite32(this_count * 4,
				  dev->ioremap_sgdma_to_fft_descriptor +
				  DESC_LENGTH_REG);
			iowrite32(START_DMA_TO_FFT_MASK,
				  dev->ioremap_sgdma_to_fft_descriptor +
				  DESC_CONTROL_REG);

			/* maintain counters and pointers */
			write_block_count--;
			dev->fft256_buffer_inflight_count++;
			dev->fft256_buffer_write_index++;
			if (dev->fft256_buffer_write_index >= 32)
				dev->fft256_buffer_write_index = 0;
		}

		while (read_block_count > 0) {
			if (dev->fft256_buffer_inflight_count == 0)
				break;

			if (dev->fft256_buffer_inflight_count >= 2) {
				/* wait for at least two descriptors to be used
				 */
				do {
					dma_desc_fill_level =
					    ioread32(dev->
						     ioremap_sgdma_from_fft_csr
						     +
						 CSR_DESCRIPTOR_FILL_LEVEL_REG);
					dma_desc_fill_level &=
					CSR_WRITE_FILL_LEVEL_MASK;
					dma_desc_fill_level >>=
				      CSR_WRITE_FILL_LEVEL_OFFSET;
				} while (dma_desc_fill_level >
					 (dev->fft256_buffer_inflight_count -
					  2));
			} else {
				/* wait for all descriptors to be used */
				do {
					dma_desc_fill_level =
					    ioread32(dev->
						     ioremap_sgdma_from_fft_csr
						     +
						 CSR_DESCRIPTOR_FILL_LEVEL_REG);
					dma_desc_fill_level &=
					CSR_WRITE_FILL_LEVEL_MASK;
					dma_desc_fill_level >>=
				      CSR_WRITE_FILL_LEVEL_OFFSET;
				} while (dma_desc_fill_level > 0);

				/* wait for read from FFT DMA to complete */
				do {
					dma_status =
					    ioread32(dev->
						     ioremap_sgdma_from_fft_csr
						     + CSR_STATUS_REG);
				} while ((dma_status &
					  CSR_BUSY_MASK) != 0);
			}

			/* move the results buffer into the user buffer */
			next_fft256_buffer_offset =
			    dev->fft256_buffer_read_index * this_count * 8;
			if (copy_to_user
			    (user_buffer,
			     dev->dma_buffer + next_fft256_buffer_offset,
			     this_count * 8)) {
				up(&dev->sem);
				pr_info
				  ("fft256stream_dev_read copy_to_user exit\n");
				return -EFAULT;
			}

			/* maintain counters and pointers */
			user_buffer += this_count * 8;
			read_count += this_count * 8;
			read_block_count--;
			dev->fft256_buffer_inflight_count--;
			dev->fft256_buffer_read_index++;
			if (dev->fft256_buffer_read_index >= 32)
				dev->fft256_buffer_read_index = 0;
		}
	}

	up(&dev->sem);
	return read_count;
}

static int fft256stream_dev_open(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = &the_fft_dev;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = -EBUSY;
	if ((dev->raw256stream_open_for_read != 0) ||
	    (dev->raw256stream_open_for_write != 0) ||
	    (dev->fft_open_for_read != 0) ||
	    (dev->fft_open_for_write != 0) ||
	    (dev->fftdma_open_for_read != 0) ||
	    (dev->fftdma_open_for_write != 0) ||
	    (dev->fft256_open_for_read != 0) ||
	    (dev->fft256_open_for_write != 0))
		goto do_exit;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		if (dev->fft256stream_open_for_read != 0)
			goto do_exit;
		dev->fft256stream_open_for_read = 1;
		break;
	case (O_WRONLY):
		if (dev->fft256stream_open_for_write != 0)
			goto do_exit;
		dev->fft256stream_open_for_write = 1;
		break;
	case (O_RDWR):
		if (dev->fft256stream_open_for_read != 0)
			goto do_exit;
		if (dev->fft256stream_open_for_write != 0)
			goto do_exit;
		dev->fft256stream_open_for_read = 1;
		dev->fft256stream_open_for_write = 1;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

	ret_val = 0;
	fp->private_data = dev;

do_exit:
	up(&dev->sem);
	return ret_val;
}

static int fft256stream_dev_release(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = fp->private_data;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = 0;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		dev->fft256stream_open_for_read = 0;
		break;
	case (O_WRONLY):
		dev->fft256stream_open_for_write = 0;
		break;
	case (O_RDWR):
		dev->fft256stream_open_for_read = 0;
		dev->fft256stream_open_for_write = 0;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

do_exit:
	up(&dev->sem);
	return ret_val;
}

static const struct file_operations fft256stream_dev_fops = {
	.owner = THIS_MODULE,
	.open = fft256stream_dev_open,
	.release = fft256stream_dev_release,
	.read = fft256stream_dev_read,
	.write = fft256stream_dev_write,
};

static struct miscdevice fft256stream_dev_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fft256stream",
	.fops = &fft256stream_dev_fops,
};

/* misc device - fft256dma_dev */
static ssize_t fft256dma_dev_write(struct file *fp,
				   const char __user *user_buffer,
				   size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	int this_count = 256;
	int write_count = 0;
	int next_fft256_buffer_offset;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/* validate count request */
	if (count & ((this_count * 4) - 1)) {
		up(&dev->sem);
		return -EINVAL;
	}

	/* set the FFT length */
	iowrite32(this_count, dev->ioremap_fft_st_adapter);

	while (count > 0) {
		if (dev->fft256_buffer_inflight_count >= 32)
			break;

		/* move the input data from the user buffer into the fft data
		   buffer */
		next_fft256_buffer_offset =
		    dev->fft256_buffer_write_index * this_count * 8;
		if (copy_from_user
		    (dev->dma_buffer + next_fft256_buffer_offset, user_buffer,
		     this_count * 4)) {
			up(&dev->sem);
			pr_info("fft256dma_dev_write copy_from_user exit\n");
			return -EFAULT;
		}

		/* start the read from FFT DMA */
		iowrite32(0x0,
			  dev->ioremap_sgdma_from_fft_descriptor +
			  DESC_READ_ADDRESS_REG);
		iowrite32(dev->dma_handle + next_fft256_buffer_offset,
			  dev->ioremap_sgdma_from_fft_descriptor +
			  DESC_WRITE_ADDRESS_REG);
		iowrite32(this_count * 8,
			  dev->ioremap_sgdma_from_fft_descriptor +
			  DESC_LENGTH_REG);
		iowrite32(START_DMA_FROM_FFT_MASK,
			  dev->ioremap_sgdma_from_fft_descriptor +
			  DESC_CONTROL_REG);

		/* start the write to FFT DMA */
		iowrite32(dev->dma_handle + next_fft256_buffer_offset,
			  dev->ioremap_sgdma_to_fft_descriptor +
			  DESC_READ_ADDRESS_REG);
		iowrite32(0x0,
			  dev->ioremap_sgdma_to_fft_descriptor +
			  DESC_WRITE_ADDRESS_REG);
		iowrite32(this_count * 4,
			  dev->ioremap_sgdma_to_fft_descriptor +
			  DESC_LENGTH_REG);
		iowrite32(START_DMA_TO_FFT_MASK,
			  dev->ioremap_sgdma_to_fft_descriptor +
			  DESC_CONTROL_REG);

		/* maintain counters and pointers */
		user_buffer += this_count * 4;
		write_count += this_count * 4;
		count -= this_count * 4;
		dev->fft256_buffer_inflight_count++;
		dev->fft256_buffer_write_index++;
		if (dev->fft256_buffer_write_index >= 32)
			dev->fft256_buffer_write_index = 0;
	}

	up(&dev->sem);
	return write_count;
}

static ssize_t fft256dma_dev_read(struct file *fp, char __user *user_buffer,
				  size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	uint32_t dma_status;
	uint32_t dma_desc_fill_level;
	int this_count = 256;
	int read_count = 0;
	int next_fft256_buffer_offset;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/* validate count request */
	if (count & ((this_count * 8) - 1)) {
		up(&dev->sem);
		return -EINVAL;
	}

	while (count > 0) {
		if (dev->fft256_buffer_inflight_count == 0)
			break;

		if (dev->fft256_buffer_inflight_count >= 2) {
			/* wait for at least two descriptors to be used */
			do {
				dma_desc_fill_level =
				    ioread32(dev->ioremap_sgdma_from_fft_csr +
					     CSR_DESCRIPTOR_FILL_LEVEL_REG);
				dma_desc_fill_level &=
				    CSR_WRITE_FILL_LEVEL_MASK;
				dma_desc_fill_level >>=
				    CSR_WRITE_FILL_LEVEL_OFFSET;
			} while (dma_desc_fill_level >
				 (dev->fft256_buffer_inflight_count - 2));
		} else {
			/* wait for all descriptors to be used */
			do {
				dma_desc_fill_level =
				    ioread32(dev->ioremap_sgdma_from_fft_csr +
					     CSR_DESCRIPTOR_FILL_LEVEL_REG);
				dma_desc_fill_level &=
				    CSR_WRITE_FILL_LEVEL_MASK;
				dma_desc_fill_level >>=
				    CSR_WRITE_FILL_LEVEL_OFFSET;
			} while (dma_desc_fill_level > 0);

			/* wait for read from FFT DMA to complete */
			do {
				dma_status =
				    ioread32(dev->ioremap_sgdma_from_fft_csr +
					     CSR_STATUS_REG);
			} while ((dma_status & CSR_BUSY_MASK) !=
				 0);
		}

		/* move the results buffer into the user buffer */
		next_fft256_buffer_offset =
		    dev->fft256_buffer_read_index * this_count * 8;
		if (copy_to_user
		    (user_buffer, dev->dma_buffer + next_fft256_buffer_offset,
		     this_count * 8)) {
			up(&dev->sem);
			pr_info("fft256dma_dev_read copy_to_user exit\n");
			return -EFAULT;
		}

		/* maintain counters and pointers */
		user_buffer += this_count * 8;
		read_count += this_count * 8;
		count -= this_count * 8;
		dev->fft256_buffer_inflight_count--;
		dev->fft256_buffer_read_index++;
		if (dev->fft256_buffer_read_index >= 32)
			dev->fft256_buffer_read_index = 0;
	}

	up(&dev->sem);
	return read_count;
}

static int fft256dma_dev_open(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = &the_fft_dev;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = -EBUSY;
	if ((dev->raw256stream_open_for_read != 0) ||
	    (dev->raw256stream_open_for_write != 0) ||
	    (dev->fft256stream_open_for_read != 0) ||
	    (dev->fft256stream_open_for_write != 0) ||
	    (dev->fft_open_for_read != 0) ||
	    (dev->fft_open_for_write != 0) ||
	    (dev->fftdma_open_for_read != 0) ||
	    (dev->fftdma_open_for_write != 0) ||
	    (dev->fft256_open_for_read != 0) ||
	    (dev->fft256_open_for_write != 0))
		goto do_exit;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		if (dev->fft256dma_open_for_read != 0)
			goto do_exit;
		dev->fft256dma_open_for_read = 1;
		break;
	case (O_WRONLY):
		if (dev->fft256dma_open_for_write != 0)
			goto do_exit;
		dev->fft256dma_open_for_write = 1;
		break;
	case (O_RDWR):
		if (dev->fft256dma_open_for_read != 0)
			goto do_exit;
		if (dev->fft256dma_open_for_write != 0)
			goto do_exit;
		dev->fft256dma_open_for_read = 1;
		dev->fft256dma_open_for_write = 1;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

	ret_val = 0;
	fp->private_data = dev;

do_exit:
	up(&dev->sem);
	return ret_val;
}

static int fft256dma_dev_release(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = fp->private_data;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = 0;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		dev->fft256dma_open_for_read = 0;
		break;
	case (O_WRONLY):
		dev->fft256dma_open_for_write = 0;
		break;
	case (O_RDWR):
		dev->fft256dma_open_for_read = 0;
		dev->fft256dma_open_for_write = 0;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

do_exit:
	up(&dev->sem);
	return ret_val;
}

static const struct file_operations fft256dma_dev_fops = {
	.owner = THIS_MODULE,
	.open = fft256dma_dev_open,
	.release = fft256dma_dev_release,
	.read = fft256dma_dev_read,
	.write = fft256dma_dev_write,
};

static struct miscdevice fft256dma_dev_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fft256dma",
	.fops = &fft256dma_dev_fops,
};

/* misc device - fftdma_dev */
static ssize_t fftdma_dev_write(struct file *fp,
				const char __user *user_buffer, size_t count,
				loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	uint32_t dma_status;
	int this_count = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	switch (count) {
	case (32 * 4):
		this_count = 32;
		break;
	case (64 * 4):
		this_count = 64;
		break;
	case (128 * 4):
		this_count = 128;
		break;
	case (256 * 4):
		this_count = 256;
		break;
	case (512 * 4):
		this_count = 512;
		break;
	case (1024 * 4):
		this_count = 1024;
		break;
	case (2048 * 4):
		this_count = 2048;
		break;
	case (4096 * 4):
		this_count = 4096;
		break;
	default:
		this_count = 0;
		break;
	}

	if (this_count == 0) {
		up(&dev->sem);
		return -EINVAL;
	}

	/* move the input data from the user buffer into the dma buffer */
	if (copy_from_user
	    (dev->dma_buffer + DMA_DATA_OFFSET, user_buffer, this_count * 4)) {
		up(&dev->sem);
		pr_info("fftdma_dev_write copy_from_user exit\n");
		return -EFAULT;
	}

	/* set the FFT length */
	iowrite32(this_count, dev->ioremap_fft_st_adapter);

	/* start the read from FFT DMA */
	iowrite32(0x0,
		  dev->ioremap_sgdma_from_fft_descriptor +
		  DESC_READ_ADDRESS_REG);
	iowrite32(dev->dma_handle + DMA_ACP_OFFSET + DMA_RESULT_OFFSET,
		  dev->ioremap_sgdma_from_fft_descriptor +
		  DESC_WRITE_ADDRESS_REG);
	iowrite32(this_count * 8,
		  dev->ioremap_sgdma_from_fft_descriptor + DESC_LENGTH_REG);
	iowrite32(START_DMA_FROM_FFT_MASK,
		  dev->ioremap_sgdma_from_fft_descriptor + DESC_CONTROL_REG);

	/* start the write to FFT DMA */
	iowrite32(dev->dma_handle + DMA_ACP_OFFSET + DMA_DATA_OFFSET,
		  dev->ioremap_sgdma_to_fft_descriptor + DESC_READ_ADDRESS_REG);
	iowrite32(0x0,
		  dev->ioremap_sgdma_to_fft_descriptor +
		  DESC_WRITE_ADDRESS_REG);
	iowrite32(this_count * 4,
		  dev->ioremap_sgdma_to_fft_descriptor + DESC_LENGTH_REG);
	iowrite32(START_DMA_TO_FFT_MASK,
		  dev->ioremap_sgdma_to_fft_descriptor + DESC_CONTROL_REG);

	/* wait for read from FFT DMA to complete */
	do {
		dma_status =
		    ioread32(dev->ioremap_sgdma_from_fft_csr + CSR_STATUS_REG);
	} while ((dma_status & CSR_BUSY_MASK) != 0);

	up(&dev->sem);
	return count;
}

static ssize_t fftdma_dev_read(struct file *fp, char __user *user_buffer,
			       size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	int this_count = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	switch (count) {
	case (32 * 8):
		this_count = 32;
		break;
	case (64 * 8):
		this_count = 64;
		break;
	case (128 * 8):
		this_count = 128;
		break;
	case (256 * 8):
		this_count = 256;
		break;
	case (512 * 8):
		this_count = 512;
		break;
	case (1024 * 8):
		this_count = 1024;
		break;
	case (2048 * 8):
		this_count = 2048;
		break;
	case (4096 * 8):
		this_count = 4096;
		break;
	default:
		this_count = 0;
		break;
	}

	if (this_count == 0) {
		up(&dev->sem);
		return -EINVAL;
	}

	/* move the results buffer into the user buffer */
	if (copy_to_user
	    (user_buffer, dev->dma_buffer + DMA_RESULT_OFFSET,
	     this_count * 8)) {
		up(&dev->sem);
		pr_info("fftdma_dev_read copy_to_user exit\n");
		return -EFAULT;
	}

	up(&dev->sem);
	return count;
}

static int fftdma_dev_open(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = &the_fft_dev;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = -EBUSY;
	if ((dev->raw256stream_open_for_read != 0) ||
	    (dev->raw256stream_open_for_write != 0) ||
	    (dev->fft256stream_open_for_read != 0) ||
	    (dev->fft256stream_open_for_write != 0) ||
	    (dev->fft256_open_for_read != 0) ||
	    (dev->fft256_open_for_write != 0) ||
	    (dev->fft_open_for_read != 0) ||
	    (dev->fft_open_for_write != 0) ||
	    (dev->fft256dma_open_for_read != 0) ||
	    (dev->fft256dma_open_for_write != 0))
		goto do_exit;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		if (dev->fftdma_open_for_read != 0)
			goto do_exit;
		dev->fftdma_open_for_read = 1;
		break;
	case (O_WRONLY):
		if (dev->fftdma_open_for_write != 0)
			goto do_exit;
		dev->fftdma_open_for_write = 1;
		break;
	case (O_RDWR):
		if (dev->fftdma_open_for_read != 0)
			goto do_exit;
		if (dev->fftdma_open_for_write != 0)
			goto do_exit;
		dev->fftdma_open_for_read = 1;
		dev->fftdma_open_for_write = 1;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

	ret_val = 0;
	fp->private_data = dev;

do_exit:
	up(&dev->sem);
	return ret_val;
}

static int fftdma_dev_release(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = fp->private_data;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = 0;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		dev->fftdma_open_for_read = 0;
		break;
	case (O_WRONLY):
		dev->fftdma_open_for_write = 0;
		break;
	case (O_RDWR):
		dev->fftdma_open_for_read = 0;
		dev->fftdma_open_for_write = 0;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

do_exit:
	up(&dev->sem);
	return ret_val;
}

static const struct file_operations fftdma_dev_fops = {
	.owner = THIS_MODULE,
	.open = fftdma_dev_open,
	.release = fftdma_dev_release,
	.read = fftdma_dev_read,
	.write = fftdma_dev_write,
};

static struct miscdevice fftdma_dev_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fftdma",
	.fops = &fftdma_dev_fops,
};

/* misc device - fft256_dev */
static ssize_t fft256_dev_write(struct file *fp,
				const char __user *user_buffer, size_t count,
				loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	int this_count = 256;
	int write_count = 0;
	int next_fft256_buffer_offset;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/* validate count request */
	if (count & ((this_count * 4) - 1)) {
		up(&dev->sem);
		return -EINVAL;
	}

	/* set the FFT length */
	iowrite32(this_count, dev->ioremap_fft_st_adapter);

	while (count > 0) {
		if (dev->fft256_buffer_inflight_count >= 32)
			break;

		/* move the input data from the user buffer into the fft data
		   buffer */
		next_fft256_buffer_offset =
		    dev->fft256_buffer_write_index * this_count * 8;
		if (copy_from_user
		    (dev->ioremap_fft_data_ram + next_fft256_buffer_offset,
		     user_buffer, this_count * 4)) {
			up(&dev->sem);
			pr_info("fft256_dev_write copy_from_user exit\n");
			return -EFAULT;
		}

		/* start the read from FFT DMA */
		iowrite32(0x0,
			  dev->ioremap_sgdma_from_fft_descriptor +
			  DESC_READ_ADDRESS_REG);
		iowrite32(DMA_RAM_DATA_ADDR + next_fft256_buffer_offset,
			  dev->ioremap_sgdma_from_fft_descriptor +
			  DESC_WRITE_ADDRESS_REG);
		iowrite32(this_count * 8,
			  dev->ioremap_sgdma_from_fft_descriptor +
			  DESC_LENGTH_REG);
		iowrite32(START_DMA_FROM_FFT_MASK,
			  dev->ioremap_sgdma_from_fft_descriptor +
			  DESC_CONTROL_REG);

		/* start the write to FFT DMA */
		iowrite32(DMA_RAM_DATA_ADDR + next_fft256_buffer_offset,
			  dev->ioremap_sgdma_to_fft_descriptor +
			  DESC_READ_ADDRESS_REG);
		iowrite32(0x0,
			  dev->ioremap_sgdma_to_fft_descriptor +
			  DESC_WRITE_ADDRESS_REG);
		iowrite32(this_count * 4,
			  dev->ioremap_sgdma_to_fft_descriptor +
			  DESC_LENGTH_REG);
		iowrite32(START_DMA_TO_FFT_MASK,
			  dev->ioremap_sgdma_to_fft_descriptor +
			  DESC_CONTROL_REG);

		/* maintain counters and pointers */
		user_buffer += this_count * 4;
		write_count += this_count * 4;
		count -= this_count * 4;
		dev->fft256_buffer_inflight_count++;
		dev->fft256_buffer_write_index++;
		if (dev->fft256_buffer_write_index >= 32)
			dev->fft256_buffer_write_index = 0;
	}

	up(&dev->sem);
	return write_count;
}

static ssize_t fft256_dev_read(struct file *fp, char __user *user_buffer,
			       size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	uint32_t dma_status;
	uint32_t dma_desc_fill_level;
	int this_count = 256;
	int read_count = 0;
	int next_fft256_buffer_offset;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/* validate count request */
	if (count & ((this_count * 8) - 1)) {
		up(&dev->sem);
		return -EINVAL;
	}

	while (count > 0) {
		if (dev->fft256_buffer_inflight_count == 0)
			break;

		if (dev->fft256_buffer_inflight_count >= 2) {
			/* wait for at least two descriptors to be used */
			do {
				dma_desc_fill_level =
				    ioread32(dev->ioremap_sgdma_from_fft_csr +
					     CSR_DESCRIPTOR_FILL_LEVEL_REG);
				dma_desc_fill_level &=
				    CSR_WRITE_FILL_LEVEL_MASK;
				dma_desc_fill_level >>=
				    CSR_WRITE_FILL_LEVEL_OFFSET;
			} while (dma_desc_fill_level >
				 (dev->fft256_buffer_inflight_count - 2));
		} else {
			/* wait for all descriptors to be used */
			do {
				dma_desc_fill_level =
				    ioread32(dev->ioremap_sgdma_from_fft_csr +
					     CSR_DESCRIPTOR_FILL_LEVEL_REG);
				dma_desc_fill_level &=
				    CSR_WRITE_FILL_LEVEL_MASK;
				dma_desc_fill_level >>=
				    CSR_WRITE_FILL_LEVEL_OFFSET;
			} while (dma_desc_fill_level > 0);

			/* wait for read from FFT DMA to complete */
			do {
				dma_status =
				    ioread32(dev->ioremap_sgdma_from_fft_csr +
					     CSR_STATUS_REG);
			} while ((dma_status & CSR_BUSY_MASK) !=
				 0);
		}

		/* move the results buffer into the user buffer */
		next_fft256_buffer_offset =
		    dev->fft256_buffer_read_index * this_count * 8;
		if (copy_to_user
		    (user_buffer,
		     dev->ioremap_fft_data_ram + next_fft256_buffer_offset,
		     this_count * 8)) {
			up(&dev->sem);
			pr_info("fft256_dev_read copy_to_user exit\n");
			return -EFAULT;
		}

		/* maintain counters and pointers */
		user_buffer += this_count * 8;
		read_count += this_count * 8;
		count -= this_count * 8;
		dev->fft256_buffer_inflight_count--;
		dev->fft256_buffer_read_index++;
		if (dev->fft256_buffer_read_index >= 32)
			dev->fft256_buffer_read_index = 0;
	}

	up(&dev->sem);
	return read_count;
}

static int fft256_dev_open(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = &the_fft_dev;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = -EBUSY;
	if ((dev->raw256stream_open_for_read != 0) ||
	    (dev->raw256stream_open_for_write != 0) ||
	    (dev->fft256stream_open_for_read != 0) ||
	    (dev->fft256stream_open_for_write != 0) ||
	    (dev->fft_open_for_read != 0) ||
	    (dev->fft_open_for_write != 0) ||
	    (dev->fftdma_open_for_read != 0) ||
	    (dev->fftdma_open_for_write != 0) ||
	    (dev->fft256dma_open_for_read != 0) ||
	    (dev->fft256dma_open_for_write != 0))
		goto do_exit;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		if (dev->fft256_open_for_read != 0)
			goto do_exit;
		dev->fft256_open_for_read = 1;
		break;
	case (O_WRONLY):
		if (dev->fft256_open_for_write != 0)
			goto do_exit;
		dev->fft256_open_for_write = 1;
		break;
	case (O_RDWR):
		if (dev->fft256_open_for_read != 0)
			goto do_exit;
		if (dev->fft256_open_for_write != 0)
			goto do_exit;
		dev->fft256_open_for_read = 1;
		dev->fft256_open_for_write = 1;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

	ret_val = 0;
	fp->private_data = dev;

do_exit:
	up(&dev->sem);
	return ret_val;
}

static int fft256_dev_release(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = fp->private_data;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = 0;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		dev->fft256_open_for_read = 0;
		break;
	case (O_WRONLY):
		dev->fft256_open_for_write = 0;
		break;
	case (O_RDWR):
		dev->fft256_open_for_read = 0;
		dev->fft256_open_for_write = 0;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

do_exit:
	up(&dev->sem);
	return ret_val;
}

static const struct file_operations fft256_dev_fops = {
	.owner = THIS_MODULE,
	.open = fft256_dev_open,
	.release = fft256_dev_release,
	.read = fft256_dev_read,
	.write = fft256_dev_write,
};

static struct miscdevice fft256_dev_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fft256",
	.fops = &fft256_dev_fops,
};

/* misc device - fft_dev */
static ssize_t fft_dev_write(struct file *fp, const char __user *user_buffer,
			     size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	uint32_t dma_status;
	int this_count = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	switch (count) {
	case (32 * 4):
		this_count = 32;
		break;
	case (64 * 4):
		this_count = 64;
		break;
	case (128 * 4):
		this_count = 128;
		break;
	case (256 * 4):
		this_count = 256;
		break;
	case (512 * 4):
		this_count = 512;
		break;
	case (1024 * 4):
		this_count = 1024;
		break;
	case (2048 * 4):
		this_count = 2048;
		break;
	case (4096 * 4):
		this_count = 4096;
		break;
	default:
		this_count = 0;
		break;
	}

	if (this_count == 0) {
		up(&dev->sem);
		return -EINVAL;
	}

	/* move the input data from the user buffer into the fft data buffer */
	if (copy_from_user
	    (dev->ioremap_fft_data_ram + DMA_DATA_OFFSET, user_buffer,
	     this_count * 4)) {
		up(&dev->sem);
		pr_info("fft_dev_write copy_from_user exit\n");
		return -EFAULT;
	}

	/* set the FFT length */
	iowrite32(this_count, dev->ioremap_fft_st_adapter);

	/* start the read from FFT DMA */
	iowrite32(0x0,
		  dev->ioremap_sgdma_from_fft_descriptor +
		  DESC_READ_ADDRESS_REG);
	iowrite32(DMA_RAM_RESULTS_ADDR,
		  dev->ioremap_sgdma_from_fft_descriptor +
		  DESC_WRITE_ADDRESS_REG);
	iowrite32(this_count * 8,
		  dev->ioremap_sgdma_from_fft_descriptor + DESC_LENGTH_REG);
	iowrite32(START_DMA_FROM_FFT_MASK,
		  dev->ioremap_sgdma_from_fft_descriptor + DESC_CONTROL_REG);

	/* start the write to FFT DMA */
	iowrite32(DMA_RAM_DATA_ADDR,
		  dev->ioremap_sgdma_to_fft_descriptor + DESC_READ_ADDRESS_REG);
	iowrite32(0x0,
		  dev->ioremap_sgdma_to_fft_descriptor +
		  DESC_WRITE_ADDRESS_REG);
	iowrite32(this_count * 4,
		  dev->ioremap_sgdma_to_fft_descriptor + DESC_LENGTH_REG);
	iowrite32(START_DMA_TO_FFT_MASK,
		  dev->ioremap_sgdma_to_fft_descriptor + DESC_CONTROL_REG);

	/* wait for read from FFT DMA to complete */
	do {
		dma_status =
		    ioread32(dev->ioremap_sgdma_from_fft_csr + CSR_STATUS_REG);
	} while ((dma_status & CSR_BUSY_MASK) != 0);

	up(&dev->sem);
	return count;
}

static ssize_t fft_dev_read(struct file *fp, char __user *user_buffer,
			    size_t count, loff_t *offset)
{
	struct fft_dev *dev = fp->private_data;
	int this_count = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	switch (count) {
	case (32 * 8):
		this_count = 32;
		break;
	case (64 * 8):
		this_count = 64;
		break;
	case (128 * 8):
		this_count = 128;
		break;
	case (256 * 8):
		this_count = 256;
		break;
	case (512 * 8):
		this_count = 512;
		break;
	case (1024 * 8):
		this_count = 1024;
		break;
	case (2048 * 8):
		this_count = 2048;
		break;
	case (4096 * 8):
		this_count = 4096;
		break;
	default:
		this_count = 0;
		break;
	}

	if (this_count == 0) {
		up(&dev->sem);
		return -EINVAL;
	}

	/* move the results buffer into the user buffer */
	if (copy_to_user
	    (user_buffer, dev->ioremap_fft_data_ram + DMA_RESULT_OFFSET,
	     this_count * 8)) {
		up(&dev->sem);
		pr_info("fft_dev_read copy_to_user exit\n");
		return -EFAULT;
	}

	up(&dev->sem);
	return count;
}

static int fft_dev_open(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = &the_fft_dev;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = -EBUSY;
	if ((dev->raw256stream_open_for_read != 0) ||
	    (dev->raw256stream_open_for_write != 0) ||
	    (dev->fft256stream_open_for_read != 0) ||
	    (dev->fft256stream_open_for_write != 0) ||
	    (dev->fft256_open_for_read != 0) ||
	    (dev->fft256_open_for_write != 0) ||
	    (dev->fftdma_open_for_read != 0) ||
	    (dev->fftdma_open_for_write != 0) ||
	    (dev->fft256dma_open_for_read != 0) ||
	    (dev->fft256dma_open_for_write != 0))
		goto do_exit;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		if (dev->fft_open_for_read != 0)
			goto do_exit;
		dev->fft_open_for_read = 1;
		break;
	case (O_WRONLY):
		if (dev->fft_open_for_write != 0)
			goto do_exit;
		dev->fft_open_for_write = 1;
		break;
	case (O_RDWR):
		if (dev->fft_open_for_read != 0)
			goto do_exit;
		if (dev->fft_open_for_write != 0)
			goto do_exit;
		dev->fft_open_for_read = 1;
		dev->fft_open_for_write = 1;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

	ret_val = 0;
	fp->private_data = dev;

do_exit:
	up(&dev->sem);
	return ret_val;
}

static int fft_dev_release(struct inode *ip, struct file *fp)
{
	struct fft_dev *dev = fp->private_data;
	int ret_val;
	uint32_t access_mode;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	ret_val = 0;

	access_mode = fp->f_flags & O_ACCMODE;
	switch (access_mode) {
	case (O_RDONLY):
		dev->fft_open_for_read = 0;
		break;
	case (O_WRONLY):
		dev->fft_open_for_write = 0;
		break;
	case (O_RDWR):
		dev->fft_open_for_read = 0;
		dev->fft_open_for_write = 0;
		break;
	default:
		ret_val = -EINVAL;
		goto do_exit;
	}

do_exit:
	up(&dev->sem);
	return ret_val;
}

static const struct file_operations fft_dev_fops = {
	.owner = THIS_MODULE,
	.open = fft_dev_open,
	.release = fft_dev_release,
	.read = fft_dev_read,
	.write = fft_dev_write,
};

static struct miscdevice fft_dev_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fft",
	.fops = &fft_dev_fops,
};

/* platform driver */
static int platform_probe(struct platform_device *pdev)
{
	int ret_val;
	struct resource *r;
	struct resource *mem_region;
	uint32_t dma_status;
	uint32_t dma_control;

	ret_val = -EBUSY;

	/* acquire the probe lock */
	if (down_interruptible(&g_dev_probe_sem))
		return -ERESTARTSYS;

	if (g_platform_probe_flag != 0)
		goto bad_exit_return;

	ret_val = -EINVAL;

	/* get our first memory resource */
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		pr_err("IORESOURCE_MEM, 0 does not exist\n");
		goto bad_exit_return;
	}

	/* initialize the device offsets and sizes */
	the_fft_dev.phys_fft_st_adapter = r->start;
	the_fft_dev.size_fft_st_adapter = 0x10;
	the_fft_dev.phys_fft_data_ram =
	    the_fft_dev.phys_fft_st_adapter - 0x10000;
	the_fft_dev.size_fft_data_ram = 0x10000;
	the_fft_dev.phys_sgdma_from_fft_descriptor =
	    the_fft_dev.phys_fft_data_ram - 0x10000;
	the_fft_dev.size_sgdma_from_fft_descriptor = 0x10;
	the_fft_dev.phys_sgdma_from_fft_csr =
	    the_fft_dev.phys_sgdma_from_fft_descriptor - 0x10000;
	the_fft_dev.size_sgdma_from_fft_csr = 0x20;
	the_fft_dev.phys_sgdma_to_fft_descriptor =
	    the_fft_dev.phys_sgdma_from_fft_csr - 0x10000;
	the_fft_dev.size_sgdma_to_fft_descriptor = 0x10;
	the_fft_dev.phys_sgdma_to_fft_csr =
	    the_fft_dev.phys_sgdma_to_fft_descriptor - 0x10000;
	the_fft_dev.size_sgdma_to_fft_csr = 0x20;

	the_fft_dev.phys_sgdma_from_ram_descriptor =
	    the_fft_dev.phys_sgdma_from_fft_descriptor + 0x8000;
	the_fft_dev.size_sgdma_from_ram_descriptor = 0x10;
	the_fft_dev.phys_sgdma_from_ram_csr =
	    the_fft_dev.phys_sgdma_from_fft_csr + 0x8000;
	the_fft_dev.size_sgdma_from_ram_csr = 0x20;

	ret_val = -EBUSY;

	/* reserve our memory regions */
	mem_region =
	    request_mem_region(the_fft_dev.phys_fft_st_adapter,
			       the_fft_dev.size_fft_st_adapter,
			       "fft_st_adapter_region");
	if (mem_region == NULL) {
		pr_err("request_mem_region failed: phys_fft_st_adapter\n");
		goto bad_exit_return;
	}

	mem_region =
	    request_mem_region(the_fft_dev.phys_fft_data_ram,
			       the_fft_dev.size_fft_data_ram,
			       "fft_data_ram_region");
	if (mem_region == NULL) {
		pr_err("request_mem_region failed: phys_fft_data_ram\n");
		goto bad_exit_release_fft_st_adapter_region;
	}

	mem_region =
	    request_mem_region(the_fft_dev.phys_sgdma_from_fft_descriptor,
			       the_fft_dev.size_sgdma_from_fft_descriptor,
			       "sgdma_from_fft_descriptor_region");
	if (mem_region == NULL) {
		pr_err
		("request_mem_region failed: phys_sgdma_from_fft_descriptor\n");
		goto bad_exit_release_fft_data_ram_region;
	}

	mem_region =
	    request_mem_region(the_fft_dev.phys_sgdma_from_fft_csr,
			       the_fft_dev.size_sgdma_from_fft_csr,
			       "sgdma_from_fft_csr_region");
	if (mem_region == NULL) {
		pr_err("request_mem_region failed: phys_sgdma_from_fft_csr\n");
		goto bad_exit_release_sgdma_from_fft_descriptor_region;
	}

	mem_region =
	    request_mem_region(the_fft_dev.phys_sgdma_to_fft_descriptor,
			       the_fft_dev.size_sgdma_to_fft_descriptor,
			       "sgdma_to_fft_descriptor_region");
	if (mem_region == NULL) {
		pr_err
		  ("request_mem_region failed: phys_sgdma_to_fft_descriptor\n");
		goto bad_exit_release_sgdma_from_fft_csr_region;
	}

	mem_region =
	    request_mem_region(the_fft_dev.phys_sgdma_to_fft_csr,
			       the_fft_dev.size_sgdma_to_fft_csr,
			       "sgdma_to_fft_csr_region");
	if (mem_region == NULL) {
		pr_err("request_mem_region failed: phys_sgdma_to_fft_csr\n");
		goto bad_exit_release_sgdma_to_fft_descriptor_region;
	}

	mem_region =
	    request_mem_region(the_fft_dev.phys_sgdma_from_ram_descriptor,
			       the_fft_dev.size_sgdma_from_ram_descriptor,
			       "sgdma_from_ram_descriptor_region");
	if (mem_region == NULL) {
		pr_err
		("request_mem_region failed: phys_sgdma_from_ram_descriptor\n");
		goto bad_exit_release_sgdma_to_fft_csr_region;
	}

	mem_region =
	    request_mem_region(the_fft_dev.phys_sgdma_from_ram_csr,
			       the_fft_dev.size_sgdma_from_ram_csr,
			       "sgdma_from_ram_csr_region");
	if (mem_region == NULL) {
		pr_err("request_mem_region failed: phys_sgdma_from_ram_csr\n");
		goto bad_exit_release_sgdma_from_ram_descriptor_region;
	}

	/* ioremap our memory regions */
	the_fft_dev.ioremap_fft_st_adapter =
	    ioremap(the_fft_dev.phys_fft_st_adapter,
		    the_fft_dev.size_fft_st_adapter);
	if (the_fft_dev.ioremap_fft_st_adapter == NULL) {
		pr_err("ioremap failed: phys_fft_st_adapter\n");
		goto bad_exit_release_sgdma_from_ram_csr_region;
	}

	the_fft_dev.ioremap_fft_data_ram =
	    ioremap(the_fft_dev.phys_fft_data_ram,
		    the_fft_dev.size_fft_data_ram);
	if (the_fft_dev.ioremap_fft_data_ram == NULL) {
		pr_err("ioremap failed: phys_fft_data_ram\n");
		goto bad_exit_iounmap_fft_st_adapter;
	}

	the_fft_dev.ioremap_sgdma_from_fft_descriptor =
	    ioremap(the_fft_dev.phys_sgdma_from_fft_descriptor,
		    the_fft_dev.size_sgdma_from_fft_descriptor);
	if (the_fft_dev.ioremap_sgdma_from_fft_descriptor == NULL) {
		pr_err("ioremap failed: phys_sgdma_from_fft_descriptor\n");
		goto bad_exit_iounmap_fft_data_ram;
	}

	the_fft_dev.ioremap_sgdma_from_fft_csr =
	    ioremap(the_fft_dev.phys_sgdma_from_fft_csr,
		    the_fft_dev.size_sgdma_from_fft_csr);
	if (the_fft_dev.ioremap_sgdma_from_fft_csr == NULL) {
		pr_err("ioremap failed: phys_sgdma_from_fft_csr\n");
		goto bad_exit_iounmap_sgdma_from_fft_descriptor;
	}

	the_fft_dev.ioremap_sgdma_to_fft_descriptor =
	    ioremap(the_fft_dev.phys_sgdma_to_fft_descriptor,
		    the_fft_dev.size_sgdma_to_fft_descriptor);
	if (the_fft_dev.ioremap_sgdma_to_fft_descriptor == NULL) {
		pr_err("ioremap failed: phys_sgdma_to_fft_descriptor\n");
		goto bad_exit_iounmap_sgdma_from_fft_csr;
	}

	the_fft_dev.ioremap_sgdma_to_fft_csr =
	    ioremap(the_fft_dev.phys_sgdma_to_fft_csr,
		    the_fft_dev.size_sgdma_to_fft_csr);
	if (the_fft_dev.ioremap_sgdma_to_fft_csr == NULL) {
		pr_err("ioremap failed: phys_sgdma_to_fft_csr\n");
		goto bad_exit_iounmap_sgdma_to_fft_descriptor;
	}

	the_fft_dev.ioremap_sgdma_from_ram_descriptor =
	    ioremap(the_fft_dev.phys_sgdma_from_ram_descriptor,
		    the_fft_dev.size_sgdma_from_ram_descriptor);
	if (the_fft_dev.ioremap_sgdma_from_ram_descriptor == NULL) {
		pr_err("ioremap failed: phys_sgdma_from_ram_descriptor\n");
		goto bad_exit_iounmap_sgdma_to_fft_csr;
	}

	the_fft_dev.ioremap_sgdma_from_ram_csr =
	    ioremap(the_fft_dev.phys_sgdma_from_ram_csr,
		    the_fft_dev.size_sgdma_from_ram_csr);
	if (the_fft_dev.ioremap_sgdma_from_ram_csr == NULL) {
		pr_err("ioremap failed: phys_sgdma_from_ram_csr\n");
		goto bad_exit_iounmap_sgdma_from_ram_descriptor;
	}

	/* initialize the DMA controllers */
	dma_status =
	    ioread32(the_fft_dev.ioremap_sgdma_from_fft_csr + CSR_STATUS_REG);
	if ((dma_status &
	     (CSR_BUSY_MASK | CSR_STOP_STATE_MASK |
	      CSR_RESET_STATE_MASK |
	      CSR_IRQ_SET_MASK)) != 0) {
		pr_err("sgdma_from_fft\n");
		pr_err("initial dma status set unexpected: 0x%08X\n",
		       dma_status);
		goto bad_exit_iounmap_sgdma_from_ram_csr;
	}

	if ((dma_status &
	     CSR_DESCRIPTOR_BUFFER_EMPTY_MASK) == 0) {
		pr_err("sgdma_from_fft\n");
		pr_err("initial dma status cleared unexpected: 0x%08X\n",
		       dma_status);
		goto bad_exit_iounmap_sgdma_from_ram_csr;
	}

	dma_control =
	    ioread32(the_fft_dev.ioremap_sgdma_from_fft_csr + CSR_CONTROL_REG);
	if ((dma_control &
	     (CSR_STOP_MASK | CSR_RESET_MASK |
	      CSR_STOP_ON_ERROR_MASK |
	      CSR_STOP_ON_EARLY_TERMINATION_MASK |
	      CSR_GLOBAL_INTERRUPT_MASK |
	      CSR_STOP_DESCRIPTORS_MASK)) != 0) {
		pr_err("sgdma_from_fft\n");
		pr_err("initial dma control set unexpected: 0x%08X\n",
		       dma_control);
		goto bad_exit_iounmap_sgdma_from_ram_csr;
	}

	dma_status =
	    ioread32(the_fft_dev.ioremap_sgdma_to_fft_csr + CSR_STATUS_REG);
	if ((dma_status &
	     (CSR_BUSY_MASK | CSR_STOP_STATE_MASK |
	      CSR_RESET_STATE_MASK |
	      CSR_IRQ_SET_MASK)) != 0) {
		pr_err("sgdma_to_fft\n");
		pr_err("initial dma status set unexpected: 0x%08X\n",
		       dma_status);
		goto bad_exit_iounmap_sgdma_from_ram_csr;
	}

	if ((dma_status &
	     CSR_DESCRIPTOR_BUFFER_EMPTY_MASK) == 0) {
		pr_err("sgdma_to_fft\n");
		pr_err("initial dma status cleared unexpected: 0x%08X\n",
		       dma_status);
		goto bad_exit_iounmap_sgdma_from_ram_csr;
	}

	dma_control =
	    ioread32(the_fft_dev.ioremap_sgdma_to_fft_csr + CSR_CONTROL_REG);
	if ((dma_control &
	     (CSR_STOP_MASK | CSR_RESET_MASK |
	      CSR_STOP_ON_ERROR_MASK |
	      CSR_STOP_ON_EARLY_TERMINATION_MASK |
	      CSR_GLOBAL_INTERRUPT_MASK |
	      CSR_STOP_DESCRIPTORS_MASK)) != 0) {
		pr_err("sgdma_to_fft\n");
		pr_err("initial dma control set unexpected: 0x%08X\n",
		       dma_control);
		goto bad_exit_iounmap_sgdma_from_ram_csr;
	}

	/* allocate dma buffer */
	ret_val = -ENOMEM;
	the_fft_dev.dma_buffer = kmalloc(DMA_BUFFER_ALLOC_SIZE, GFP_KERNEL);
	if (the_fft_dev.dma_buffer == NULL) {
		pr_err("kmalloc dma_buffer failed\n");
		goto bad_exit_iounmap_sgdma_from_ram_csr;
	}

	the_fft_dev.dma_handle =
	    dma_map_single(&pdev->dev, the_fft_dev.dma_buffer,
			   DMA_BUFFER_ALLOC_SIZE, DMA_BIDIRECTIONAL);

	if (dma_mapping_error(&pdev->dev, the_fft_dev.dma_handle)) {
		pr_info("dma mapping error exit\n");
		goto bad_exit_kfree;
	}

	if ((the_fft_dev.dma_handle & (0xC0000000)) != 0) {
		pr_info("dma_handle out of range exit\n");
		goto bad_exit_dma_unmap;
	}

	/* register misc device fft_dev */
	sema_init(&the_fft_dev.sem, 1);
	the_fft_dev.fft_open_for_read = 0;
	the_fft_dev.fft_open_for_write = 0;
	the_fft_dev.fft256_open_for_read = 0;
	the_fft_dev.fft256_open_for_write = 0;
	the_fft_dev.fftdma_open_for_read = 0;
	the_fft_dev.fftdma_open_for_write = 0;
	the_fft_dev.fft256dma_open_for_read = 0;
	the_fft_dev.fft256dma_open_for_write = 0;
	the_fft_dev.fft256stream_open_for_read = 0;
	the_fft_dev.fft256stream_open_for_write = 0;
	the_fft_dev.raw256stream_open_for_read = 0;
	the_fft_dev.raw256stream_open_for_write = 0;
	the_fft_dev.fft256_buffer_read_index = 0;
	the_fft_dev.fft256_buffer_write_index = 0;
	the_fft_dev.fft256_buffer_inflight_count = 0;

	ret_val = misc_register(&fft_dev_device);
	if (ret_val != 0) {
		pr_warn("Could not register device \"fft\"...");
		goto bad_exit_dma_unmap;
	}

	/* register misc device fft256_dev */
	ret_val = misc_register(&fft256_dev_device);
	if (ret_val != 0) {
		pr_warn("Could not register device \"fft256\"...");
		goto bad_exit_deregister_fft_dev;
	}

	/* register misc device fftdma_dev */
	ret_val = misc_register(&fftdma_dev_device);
	if (ret_val != 0) {
		pr_warn("Could not register device \"fftdma\"...");
		goto bad_exit_deregister_fft256_dev;
	}

	/* register misc device fft256dma_dev */
	ret_val = misc_register(&fft256dma_dev_device);
	if (ret_val != 0) {
		pr_warn("Could not register device \"fft256dma\"...");
		goto bad_exit_deregister_fftdma_dev;
	}

	/* register misc device fft256stream_dev */
	ret_val = misc_register(&fft256stream_dev_device);
	if (ret_val != 0) {
		pr_warn("Could not register device \"fft256stream\"...");
		goto bad_exit_deregister_fft256dma_dev;
	}

	/* register misc device raw256stream_dev */
	ret_val = misc_register(&raw256stream_dev_device);
	if (ret_val != 0) {
		pr_warn("Could not register device \"raw256stream\"...");
		goto bad_exit_deregister_fft256stream_dev;
	}

	g_platform_probe_flag = 1;
	up(&g_dev_probe_sem);
	return 0;

bad_exit_deregister_fft256stream_dev:
	misc_deregister(&fft256stream_dev_device);
bad_exit_deregister_fft256dma_dev:
	misc_deregister(&fft256dma_dev_device);
bad_exit_deregister_fftdma_dev:
	misc_deregister(&fftdma_dev_device);
bad_exit_deregister_fft256_dev:
	misc_deregister(&fft256_dev_device);
bad_exit_deregister_fft_dev:
	misc_deregister(&fft_dev_device);
bad_exit_dma_unmap:
	dma_unmap_single(&pdev->dev, the_fft_dev.dma_handle,
			 DMA_BUFFER_ALLOC_SIZE, DMA_BIDIRECTIONAL);
bad_exit_kfree:
	kfree(the_fft_dev.dma_buffer);
bad_exit_iounmap_sgdma_from_ram_csr:
	iounmap(the_fft_dev.ioremap_sgdma_from_ram_csr);
bad_exit_iounmap_sgdma_from_ram_descriptor:
	iounmap(the_fft_dev.ioremap_sgdma_from_ram_descriptor);
bad_exit_iounmap_sgdma_to_fft_csr:
	iounmap(the_fft_dev.ioremap_sgdma_to_fft_csr);
bad_exit_iounmap_sgdma_to_fft_descriptor:
	iounmap(the_fft_dev.ioremap_sgdma_to_fft_descriptor);
bad_exit_iounmap_sgdma_from_fft_csr:
	iounmap(the_fft_dev.ioremap_sgdma_from_fft_csr);
bad_exit_iounmap_sgdma_from_fft_descriptor:
	iounmap(the_fft_dev.ioremap_sgdma_from_fft_descriptor);
bad_exit_iounmap_fft_data_ram:
	iounmap(the_fft_dev.ioremap_fft_data_ram);
bad_exit_iounmap_fft_st_adapter:
	iounmap(the_fft_dev.ioremap_fft_st_adapter);
bad_exit_release_sgdma_from_ram_csr_region:
	release_mem_region(the_fft_dev.phys_sgdma_from_ram_csr,
			   the_fft_dev.size_sgdma_from_ram_csr);
bad_exit_release_sgdma_from_ram_descriptor_region:
	release_mem_region(the_fft_dev.phys_sgdma_from_ram_descriptor,
			   the_fft_dev.size_sgdma_from_ram_descriptor);
bad_exit_release_sgdma_to_fft_csr_region:
	release_mem_region(the_fft_dev.phys_sgdma_to_fft_csr,
			   the_fft_dev.size_sgdma_to_fft_csr);
bad_exit_release_sgdma_to_fft_descriptor_region:
	release_mem_region(the_fft_dev.phys_sgdma_to_fft_descriptor,
			   the_fft_dev.size_sgdma_to_fft_descriptor);
bad_exit_release_sgdma_from_fft_csr_region:
	release_mem_region(the_fft_dev.phys_sgdma_from_fft_csr,
			   the_fft_dev.size_sgdma_from_fft_csr);
bad_exit_release_sgdma_from_fft_descriptor_region:
	release_mem_region(the_fft_dev.phys_sgdma_from_fft_descriptor,
			   the_fft_dev.size_sgdma_from_fft_descriptor);
bad_exit_release_fft_data_ram_region:
	release_mem_region(the_fft_dev.phys_fft_data_ram,
			   the_fft_dev.size_fft_data_ram);
bad_exit_release_fft_st_adapter_region:
	release_mem_region(the_fft_dev.phys_fft_st_adapter,
			   the_fft_dev.size_fft_st_adapter);
bad_exit_return:
	up(&g_dev_probe_sem);
	return ret_val;
}

static int platform_remove(struct platform_device *pdev)
{
	misc_deregister(&raw256stream_dev_device);
	misc_deregister(&fft256stream_dev_device);
	misc_deregister(&fft256dma_dev_device);
	misc_deregister(&fftdma_dev_device);
	misc_deregister(&fft256_dev_device);
	misc_deregister(&fft_dev_device);
	dma_unmap_single(&pdev->dev, the_fft_dev.dma_handle,
			 DMA_BUFFER_ALLOC_SIZE, DMA_BIDIRECTIONAL);
	kfree(the_fft_dev.dma_buffer);
	iounmap(the_fft_dev.ioremap_sgdma_from_ram_csr);
	iounmap(the_fft_dev.ioremap_sgdma_from_ram_descriptor);
	iounmap(the_fft_dev.ioremap_sgdma_to_fft_csr);
	iounmap(the_fft_dev.ioremap_sgdma_to_fft_descriptor);
	iounmap(the_fft_dev.ioremap_sgdma_from_fft_csr);
	iounmap(the_fft_dev.ioremap_sgdma_from_fft_descriptor);
	iounmap(the_fft_dev.ioremap_fft_data_ram);
	iounmap(the_fft_dev.ioremap_fft_st_adapter);
	release_mem_region(the_fft_dev.phys_sgdma_from_ram_csr,
			   the_fft_dev.size_sgdma_from_ram_csr);
	release_mem_region(the_fft_dev.phys_sgdma_from_ram_descriptor,
			   the_fft_dev.size_sgdma_from_ram_descriptor);
	release_mem_region(the_fft_dev.phys_sgdma_to_fft_csr,
			   the_fft_dev.size_sgdma_to_fft_csr);
	release_mem_region(the_fft_dev.phys_sgdma_to_fft_descriptor,
			   the_fft_dev.size_sgdma_to_fft_descriptor);
	release_mem_region(the_fft_dev.phys_sgdma_from_fft_csr,
			   the_fft_dev.size_sgdma_from_fft_csr);
	release_mem_region(the_fft_dev.phys_sgdma_from_fft_descriptor,
			   the_fft_dev.size_sgdma_from_fft_descriptor);
	release_mem_region(the_fft_dev.phys_fft_data_ram,
			   the_fft_dev.size_fft_data_ram);
	release_mem_region(the_fft_dev.phys_fft_st_adapter,
			   the_fft_dev.size_fft_st_adapter);

	if (down_interruptible(&g_dev_probe_sem))
		return -ERESTARTSYS;

	g_platform_probe_flag = 0;
	up(&g_dev_probe_sem);

	return 0;
}

static struct of_device_id fft_driver_dt_ids[] = {
	{
	 .compatible = "altr,fft_stadapter"},
	{ /* end of table */ }
};

MODULE_DEVICE_TABLE(of, fft_driver_dt_ids);

static struct platform_driver the_platform_driver = {
	.probe = platform_probe,
	.remove = platform_remove,
	.driver = {
		   .name = "fft_driver",
		   .owner = THIS_MODULE,
		   .of_match_table = fft_driver_dt_ids,
		   },
};

static int demo_init(void)
{
	int ret_val;

	sema_init(&g_dev_probe_sem, 1);

	ret_val = platform_driver_register(&the_platform_driver);
	if (ret_val != 0) {
		pr_err("platform_driver_register returned %d\n", ret_val);
		return ret_val;
	}

	return 0;
}

static void demo_exit(void)
{
	platform_driver_unregister(&the_platform_driver);
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rod Frazer <rod.frazer@intel.com>");
MODULE_DESCRIPTION("Driver for custom Qsys FFT subsystem.");
MODULE_VERSION("1.0");
