#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#define MYALU_S00_AXI_SLV_BASE_ADDR 0x43C00000

#define MYALU_S00_AXI_SLV_REG0_OFFSET 0U
#define MYALU_S00_AXI_SLV_REG1_OFFSET 4U
#define MYALU_S00_AXI_SLV_REG2_OFFSET 8U
#define MYALU_S00_AXI_SLV_REG3_OFFSET 12U
#define MYALU_S00_AXI_SLV_REG4_OFFSET 16U
#define MYALU_S00_AXI_SLV_REG5_OFFSET 20U
#define MYALU_S00_AXI_SIZE 24U
#define MYALU_S00_AXI_LAST_ADDR (MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SIZE)

dev_t dev = 0;
static struct class *dev_class;
static struct cdev myalu_cdev;

static void __iomem *registers;

static int __init myalu_driver_init(void);
static void __exit myalu_driver_exit(void);
static int myalu_open(struct inode *inode, struct file *file);
static int myalu_release(struct inode *inode, struct file *file);
static ssize_t myalu_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t myalu_write(struct file *filp, const char *buf, size_t len, loff_t *off);

// static void axi_little_write(u32 addr, u32 data);
// static u32 axi_little_read(u32 addr);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = myalu_read,
	.write = myalu_write,
	.open = myalu_open,
	.release = myalu_release,
};

static int myalu_open(struct inode *inode, struct file *file)
{
	u32 ready = ioread32(registers + MYALU_S00_AXI_SLV_REG1_OFFSET);
	ready = ready >> 1;
	if (0x00000000 != ready)
	{
		pr_info("myalu IPCORE ready for work.\n");
	}
	else
	{
		pr_info("myalu IPCORE is bussy.\n");
	}
	return 0;
}

static int myalu_release(struct inode *inode, struct file *file)
{
	u32 ready = ioread32(registers + MYALU_S00_AXI_SLV_REG1_OFFSET);
	ready = ready >> 1;
	if (0x00000000 != ready)
	{
		pr_info("myalu IPCORE ready for work.\n");
	}
	else
	{
		pr_info("myalu IPCORE is bussy.\n");
	}
	return 0;
}

static ssize_t myalu_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	u32 result = ioread32(registers + MYALU_S00_AXI_SLV_REG5_OFFSET);
	u32 carry = ioread32(registers + MYALU_S00_AXI_SLV_REG1_OFFSET);
	carry = carry & (0x00000001);
	pr_info("%d,%d\n", carry, result);
	return 0;
}

static ssize_t myalu_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	pr_info("Driver Write Function Called...!!!\n");
	// TODO: parcear para enviar operación, operando1 y operando2
	return len;
}

static int __init myalu_driver_init(void)
{
	// void *ioremap(unsigned long phys_addr, unsigned long size);
	/*Allocating Major number*/
	if ((alloc_chrdev_region(&dev, 0, 1, "myalu")) < 0)
	{
		pr_err("Cannot allocate major number\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	/*Creating cdev structure*/
	cdev_init(&myalu_cdev, &fops);

	/*Adding character device to the system*/
	if ((cdev_add(&myalu_cdev, dev, 1)) < 0)
	{
		pr_err("Cannot add the device to the system\n");
		goto r_class;
	}

	/*Creating struct class*/
	if (IS_ERR(dev_class = class_create(THIS_MODULE, "myalu_class")))
	{
		pr_err("Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "myalu_device")))
	{
		pr_err("Cannot create the Device 1\n");
		goto r_device;
	}
	pr_info("starting myalu IPCORE.\n");
	registers = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR, MYALU_S00_AXI_SIZE);
	iowrite32(0x00000001, registers + MYALU_S00_AXI_SLV_REG0_OFFSET);
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
}

static void __exit myalu_driver_exit(void)
{
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&myalu_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("stoping myalu IPCORE\n");
	iowrite32(0x00000002, registers + MYALU_S00_AXI_SLV_REG0_OFFSET);
	iounmap(registers);
}

module_init(myalu_driver_init);
module_exit(myalu_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gonzalo Nahuel Vaca <vacagonzalo@gmail.com>");
MODULE_DESCRIPTION("myalumodule - module to communicate with myalu IPCORE.");
MODULE_VERSION("1:0.0");
