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

/*
 * PERIPHBASE and Global Timer offset
 */
#define PERIPHBASE		(0xFFFEC000)
#define GLOBAL_TIMER_OFFSET	(0x200)

/*
 * msgdma driver values
 */
#define DMA_BUFFER_ALLOC_SIZE	(64 *1024)
#define DMA_ACP_OFFSET		(0x40000000)

#define DMA_DATA_OFFSET		(0 * 1024)
#define DMA_RESULT_OFFSET	(32 * 1024)
#define DMA_RESULT_SPAN		(32 * 1024)

#define DMA_DATA_RAM_OFFSET	(0x40000)
#define DMA_RAM_DATA_ADDR	(DMA_DATA_RAM_OFFSET + DMA_DATA_OFFSET)
#define DMA_RAM_RESULTS_ADDR	(DMA_DATA_RAM_OFFSET + DMA_RESULT_OFFSET)

#define CSR_STATUS_REG			(0x0 << 2)
#define CSR_CONTROL_REG			(0x1 << 2)
#define CSR_DESCRIPTOR_FILL_LEVEL_REG	(0x2 << 2)
#define CSR_RESPONSE_FILL_LEVEL_REG	(0x3 << 2)

#define DESC_READ_ADDRESS_REG		(0x0 << 2)
#define DESC_WRITE_ADDRESS_REG		(0x1 << 2)
#define DESC_LENGTH_REG			(0x2 << 2)
#define DESC_CONTROL_REG		(0x3 << 2)

#define DESC_CTRL_GENERATE_SOP_MASK	(1 << 8)
#define DESC_CTRL_GENERATE_EOP_MASK	(1 << 9)
#define DESC_CTRL_END_ON_EOP_MASK	(1 << 12)
#define DESC_CTRL_GO_MASK		(1 << 31)

#define CSR_BUSY_MASK                           1
#define CSR_DESCRIPTOR_BUFFER_EMPTY_MASK        (1 << 1)
#define CSR_GLOBAL_INTERRUPT_MASK               (1 << 4)
#define CSR_IRQ_SET_MASK                        (1 << 9)
#define CSR_RESET_MASK                          (1 << 1)
#define CSR_RESET_STATE_MASK                    (1 << 6)
#define CSR_STOP_DESCRIPTORS_MASK               (1 << 5)
#define CSR_STOP_MASK                           1
#define CSR_STOP_ON_EARLY_TERMINATION_MASK      (1 << 3)
#define CSR_STOP_ON_ERROR_MASK                  (1 << 2)
#define CSR_STOP_STATE_MASK                     (1 << 5)
#define CSR_WRITE_FILL_LEVEL_MASK               0xFFFF0000
#define CSR_WRITE_FILL_LEVEL_OFFSET             16

#define START_DMA_FROM_FFT_MASK	( \
		DESC_CTRL_GO_MASK | \
		DESC_CTRL_END_ON_EOP_MASK)

#define START_DMA_TO_FFT_MASK	( \
		DESC_CTRL_GO_MASK | \
		DESC_CTRL_GENERATE_SOP_MASK |\
		DESC_CTRL_GENERATE_EOP_MASK)

