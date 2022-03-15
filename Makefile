# Makefile for my pcie driver
    
TARGET		:= MY_PCIE.ko
KERNEL_VERSION	= $(shell uname -r)
SRCS		= my_pcie.c

all:		${TARGET}
obj-m		:= MY_PCIE.o
MY_PCIE-objs	:= my_pcie.o 

MY_PCIE.ko: $(SRCS)
		make -C /lib/modules/$(KERNEL_VERSION)/build SUBDIRS=$(PWD) V=1 modules 
		rm -r *.o *.mod.c .*.*.cmd

clean:
		rm -f -r *.ko *.o *.mod.[co] *~ .*.*.cmd Module* modules* tmp

install:
		/sbin/insmod MY_PCIE.ko
		mknod -m  a+rw /dev/MY_PCIE0 c `cat /proc/devices | awk '$$2=="MY_PCIE" {print $$1}'` 0

uninstall:
		rm -f /dev/MY_PCIE*
		/sbin/rmmod MY_PCIE
