/************************************************************************
 *                         my_pci.c                                     *
************************************************************************/
#define __my_pcie__
#include "my_pcie.h"
#undef  __my_pcie__
#include <linux/jiffies.h>
#include <linux/uaccess.h>

static dev_t  dev_first;
struct cdev  *pcie_cdev;

static struct file_operations fops = {
  .owner     = THIS_MODULE,
  .open      = pcie_dev_open,
  .release   = pcie_dev_close,
  .ioctl     = pcie_dev_ioctl,
};

static int pcie_dev_read_ulong(struct dev_private *pdev, unsigned long arg);
static int pcie_dev_write_ulong(struct dev_private *pdev, unsigned long arg);
static int pcie_dev_open(struct inode *i_node, struct file *fp);
static int pcie_dev_close(struct inode *i_node, struct file *fp);
static long pcie_dev_ioctl(struct file *fp, unsigned int cmd, unsigned long arg);

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
  cdev_del(pciex_cdev);
  pci_unregister_driver(&pcie_dev_driver);

  // remove proc file in /proc
  // proc file records process system information
  remove_proc_entry(PCIEX_DEVNAME, NULL);
  
  printk(PCIEX_LOGPFX"Exit pcie_dev_exit function\n");
}

module_init(pcie_dev_init);
module_exit(pcie_dev_exit);

static int pcie_dev_open(struct inode *i_node, struct file *fp)
{

}

static int pcie_dev_read_ulong(struct dev_private *pdev, unsigned long arg)
{
  printk(PCIEX_LOGPFX"Enter pcie_dev_read_ulong function\n");
  // access_ok() to checks if a user space pointer is valid
  // copy from user to get parameters
  // ioread
  // copy data to user
}

static int pcie_dev_write_ulong(struct dev_private *pdev, unsigned long arg)
{
  printk(PCIEX_LOGPFX"Enter pcie_dev_write_ulong function\n");
  // access_ok() to checks if a user space pointer is valid
  // copy from user to get parameters and data
  // ioread
  // copy data to user
}