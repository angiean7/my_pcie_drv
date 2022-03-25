#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/pci.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>

#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <asm/pgtable.h>

#define PCIEX_DEVNAME "MY_PCIE"
#define PCIEX_LOGPFX "[MY_PCIE] Log: "
#define PCIEX_ERRPFX "[MY_PCIE] Err: "
#define MAX_BOARD_NUM   	1
#define MAX_DMA_CHANNEL 2
#define PCI_TYPE0_ADDRESSES 6
#define PCI_TYPE1_ADDRESSES 2
#define PCI_TYPE2_ADDRESSES 5
#define TYPE_MEM    0x00
#define TYPE_IO     0x01

#ifndef u64
#define u64 unsigned long long
#endif

#ifndef u32
#define u32 unsigned int
#endif

#ifndef u16
#define u16 unsigned short
#endif

#ifndef u8
#define u8  unsigned char
#endif

typedef	struct st_IOMSG{
  u32	Bar;
  u32	Offset;
  u32	Data;
}IOMSG, *PIOMSG;

struct st_cfg{
  u32     slot;
  u32     func;
  u32     irq;
  u32     typ[PCI_TYPE0_ADDRESSES];
  u32     bar[PCI_TYPE0_ADDRESSES];
  void   *map[PCI_TYPE0_ADDRESSES]; 
  u32     siz[PCI_TYPE0_ADDRESSES];
  u8      using_base_num;
}cfg;

struct dma_ctrl{
  u32  DmaFlag;
  u32  DmaSize;
  u32  DmaDir;
  u32  DmaTimeout;
  u32  DmaCancelFlag;
  struct semaphore  DmaSem;
  wait_queue_head_t DmaWaitQueue;
};

struct dev_private{
  struct pci_dev  	*pci;
  u32              	open_count;
  spinlock_t       	lock;
  struct semaphore 	dev_sem;
  struct st_cfg    	cfg;
  struct dma_ctrl	DmaCtrl[MAX_DMA_CHANNEL];
  wait_queue_head_t     UserIntWaitQueue;
  u32			UserIntStatus;
};



#define ERR_NO_ERROR            0x00000000
/*Base error code*/
#define ERR_CODE_BASE          (-10000)
/*Parameter and DMA error*/
#define ERR_NO_DEVICE          (ERR_CODE_BASE+1)
#define ERR_INVALID_FD         (ERR_CODE_BASE+2)
#define ERR_NULL_PTR           (ERR_CODE_BASE+3)
#define ERR_BAR_OVERFLOW       (ERR_CODE_BASE+4)
#define ERR_DMA_SIZE_OVERFLOW  (ERR_CODE_BASE+5)
#define ERR_DMA_SIZE_ALIGN     (ERR_CODE_BASE+6)
#define ERR_DMA_CANCELLED      (ERR_CODE_BASE+7)
#define ERR_DMA_TIMEOUT        (ERR_CODE_BASE+8)
#define ERR_OUT_RANGE          (ERR_CODE_BASE+9)
#define ERR_ADDR_ALIGN         (ERR_CODE_BASE+10)

/*Driver inner error*/
#define ERR_DRIVER_ERROR       (ERR_CODE_BASE+100)
#define ERR_IOCTL_DATAINVAL    (ERR_DRIVER_ERROR+1)
#define ERR_DMA_MEM_ALLOC      (ERR_DRIVER_ERROR+2)
#define ERR_DMA_MEM_NULL       (ERR_DRIVER_ERROR+3)
#define ERR_AREA_VERIFY        (ERR_DRIVER_ERROR+4)
#define ERR_PCICONF_MEMALLOC   (ERR_DRIVER_ERROR+5)

#define ERR_DEVINT_INT         (ERR_DRIVER_ERROR+6) 
#define ERR_DEVINT_DEVENABLE   (ERR_DRIVER_ERROR+7) 
#define ERR_DEVINT_IO          (ERR_DRIVER_ERROR+8)
#define ERR_DEVINT_KMALLOC     (ERR_DRIVER_ERROR+9) 
#define ERR_DEVINT_IRQ         (ERR_DRIVER_ERROR+10)
#define ERR_DEVINT_DEVHAND     (ERR_DRIVER_ERROR+11) 

#define ERR_OPEN_ENTER         (ERR_DRIVER_ERROR+12)

#define ERR_COPY_FROM_USER     (ERR_DRIVER_ERROR+13)
#define ERR_COPY_TO_USER       (ERR_DRIVER_ERROR+14)

/**********************************************************************************
                            IO Memory Read & Write Functions
**********************************************************************************/
extern inline void reg_write_ulong( struct dev_private *pdev,
				    u8 bar,
				    u32 offset,
				    u32 data )
{
  iowrite32(data, (u32 *)(pdev->cfg.map[bar] + offset));
}

extern inline void reg_write_ushort( struct dev_private *pdev,
				     u8 bar,
				     u32 offset,
				     u16 data )
{
  iowrite16(data, (u16 *)(pdev->cfg.map[bar] + offset));
}

extern inline void reg_write_uchar( struct dev_private *pdev,
				    u8 bar,
				    u32 offset,
				    u8 data )
{
  iowrite8(data, (u8 *)(pdev->cfg.map[bar] + offset));
}

extern inline void reg_read_ulong( struct dev_private *pdev,
				   u8 bar,
				   u32 offset,
				   u32 *data )
{
  *data = ioread32((u32 *)(pdev->cfg.map[bar] + offset));
}

extern inline void reg_read_ushort( struct dev_private *pdev,
				    u8 bar,
				    u32 offset,
				    u16 *data )
{
  *data = ioread16((u16 *)(pdev->cfg.map[bar] + offset));
}

extern inline void reg_read_uchar( struct dev_private *pdev,
				   u8 bar,
				   u32 offset,
				   u8 *data )
{
  *data = ioread8((u8 *)(pdev->cfg.map[bar] + offset));
}