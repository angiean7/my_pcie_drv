/************************************************************************
 *                         my_pci.c                                     *
************************************************************************/
#include "my_pcie.h"
#include <linux/jiffies.h>
#include <linux/uaccess.h>

#define  PCIE_DEV_VENDOR_ID  0x10ee  
#define  PCIE_DEV_DEVICE_ID  0x7024  

static dev_t  dev_first;
struct cdev  *pcie_cdev;

static int pcie_dev_read_ulong(struct dev_private *pdev, unsigned long arg);
static int pcie_dev_write_ulong(struct dev_private *pdev, unsigned long arg);
static int pcie_dev_open(struct inode *i_node, struct file *fp);
static int pcie_dev_close(struct inode *i_node, struct file *fp);
static long pcie_dev_ioctl(struct file *fp, unsigned int cmd, unsigned long arg);

static int  pcie_dev_probe(struct pci_dev *dev, const struct pci_device_id*id);
static void pcie_dev_remove(struct pci_dev *dev);
static int pcie_dev_open(struct inode *i_node, struct file *fp);
static int pcie_dev_close(struct inode *i_node, struct file *fp);

static int pcie_dev_suspend(struct pci_dev *dev, pm_message_t state);
static int pcie_dev_resume(struct pci_dev *dev);

static struct pci_device_id pcie_dev_table []  = {
  {
    .vendor	=  PCIE_DEV_VENDOR_ID,
    .device	=  PCIE_DEV_DEVICE_ID,
    .subvendor	=  PCI_ANY_ID,
    .subdevice	=  PCI_ANY_ID,
  },
  { 
    /* all zeroes */ 
  }
};

static struct pci_driver pcie_dev_driver = {
  .name     = PCIEX_DEVNAME,
  .id_table = pcie_dev_table,
  .probe    = pcie_dev_probe,
  .remove   = pcie_dev_remove,
  .suspend  = pcie_dev_suspend,
  .resume   = pcie_dev_resume,
};

static struct file_operations fops = {
  .owner     = THIS_MODULE,
  .open      = pcie_dev_open,
  .release   = pcie_dev_close,
  .ioctl     = pcie_dev_ioctl,
};


static int __init pcie_dev_init(void)
{
  int rc;

  printk(PCIEX_LOGPFX"Enter pcie_dev_init function\n");
  
  // register a range of char device numbers
  rc = alloc_chrdev_region(&dev_first, 0, MAX_BOARD_NUM, PCIEX_DEVNAME);
  if(rc < 0)
  {
    printk(PCIEX_ERRPFX"Function failed:alloc_chrdev_region\n");
    return rc;
  }
  
  // alloc a char device
  pcie_cdev = cdev_alloc();
  pcie_cdev->ops = &fops;
  cdev_init(pcie_cdev, &fops);
  cdev_add(pcie_cdev, dev_first, 1);
  
  rc = pci_register_driver(&pcie_dev_driver);
  if(rc < 0)
  {
    printk(PCIEX_ERRPFX"Function failed:pci_register_driver\n");
  }
  
  printk(PCIEX_LOGPFX"Exit pcie_dev_init function\n");
  
  return (rc);
}

static void __exit pcie_dev_exit(void)
{  
  printk(PCIEX_LOGPFX"Enter pcie_dev_exit function\n");

  unregister_chrdev_region(dev_first, MAX_BOARD_NUM);
  cdev_del(pcie_cdev);
  pci_unregister_driver(&pcie_dev_driver);

  // remove proc file in /proc
  // proc file records process system information
  remove_proc_entry(PCIEX_DEVNAME, NULL);
  
  printk(PCIEX_LOGPFX"Exit pcie_dev_exit function\n");
}

module_init(pcie_dev_init);
module_exit(pcie_dev_exit);
/*
static int pcie_dev_open(struct inode *i_node, struct file *fp)
{
  int minor = 0;

  printk(PCIEX_LOGPFX"Enter pcie_dev_open function\n");

  // get minor number
  minor = MINOR(i_node->i_rdev);
  if(minor >= MAX_BOARD_NUM)
  {
    //return -ERESTARTSYS;;
  }

  if(NULL == (pdev = pdev_all[minor]))
  {
    return -ERESTARTSYS;;
  }
  
  fp->private_data = pdev;
  
  if(down_interruptible(&pdev->dev_sem))
  {
    return -ERESTARTSYS;
  }
*/
  //get irq


//}

// static int pcie_dev_read_ulong(struct dev_private *pdev, unsigned long arg)
// {
//   printk(PCIEX_LOGPFX"Enter pcie_dev_read_ulong function\n");
//   // access_ok() to checks if a user space pointer is valid
//   // copy from user to get parameters
//   // ioread
//   // copy data to user
// }

// static int pcie_dev_write_ulong(struct dev_private *pdev, unsigned long arg)
// {
//   printk(PCIEX_LOGPFX"Enter pcie_dev_write_ulong function\n");
//   IOMSG IoMsg;
//   u32   ret;

//   // access_ok() to checks if a user space pointer is valid
//   ret = access_ok(VERIFY_WRITE, (void *)arg, sizeof(IOMSG));
//   if(!ret)
//   {
//     printk(PCIEX_ERRPFX"Failed to verify area (status=%d)\n", ret);  
//     return ERR_AREA_VERIFY;
//   }

//   // copy from user to get parameters and data
//   ret = copy_from_user(&IoMsg, (void *)arg, sizeof(IOMSG));
//   if(ret != 0)
//   {
//     printk(PCIEX_ERRPFX"Get parameters failed!\n"); 
//     return ERR_COPY_FROM_USER;
//   }

//   // ioread
//   reg_read_ulong(pdev, IoMsg.Bar, IoMsg.Offset, (u32*)&(IoMsg.Data));

//   // copy data to user
//   ret = copy_to_user((void *)arg, &IoMsg, sizeof(IOMSG));
//   if(ret != 0)
//   {
//     printk(PCIEX_ERRPFX"Set parameters failed!\n"); 
//     return ERR_COPY_TO_USER;
//   }

//   return 0;
// }