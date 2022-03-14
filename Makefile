# Makefile for my pcie driver

DEBUG = y

ifeq ($(DEBUG),y)
    DBGFLAGS = -D_SG_DMA_DEBUG_ 
else
    DBGFLAGS =
endif
    EXTRA_CFLAGS += $(DBGFLAGS)
    
TARGET		:= MY_PCIE.ko
KERNEL_VERSION	= $(shell uname -r)
SRCS		= my_pcie.c

all:		${TARGET}
obj-m		:= MY_PCIE.o
SG_DMA-objs	:= my_pcie.o 

MY_PCIE.ko: $(SRCS)
		make -C /lib/modules/$(KERNEL_VERSION)/build SUBDIRS=$(PWD) V=1 modules 
		rm -r *.o *.mod.c .*.*.cmd

clean:
		rm -f -r *.ko *.o *.mod.[co] *~ .*.*.cmd Module* modules* tmp

install:
		/sbin/insmod SG_DMA.ko
		mknod -m  a+rw /dev/SG_DMA0 c `cat /proc/devices | awk '$$2=="SG_DMA" {print $$1}'` 0

uninstall:
		rm -f /dev/SG_DMA*
		/sbin/rmmod SG_DMA
