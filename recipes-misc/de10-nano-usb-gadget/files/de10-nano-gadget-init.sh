#!/bin/sh

modprobe g_multi file=/usr/share/de10-nano-usb-gadget/fat_image.img

sleep 5

rm /var/lib/misc/udhcpd.leases
/sbin/ifconfig usb0 hw ether 00:07:ed:01:02:03
/sbin/ifconfig usb0 192.168.7.1 netmask 255.255.255.0
/usr/sbin/udhcpd -fS -I 192.168.7.1 /etc/udhcpd.conf
