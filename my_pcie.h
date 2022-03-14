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

typedef	struct st_IOMSG{
  u32	Bar;
  u32	Offset;
  u32	Data;
}IOMSG, *PIOMSG;

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