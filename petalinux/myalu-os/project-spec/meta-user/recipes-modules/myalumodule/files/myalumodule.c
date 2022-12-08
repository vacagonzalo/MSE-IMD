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
#define MYALU_S00_AXI_SLV_REG_SIZE 32U

dev_t dev = 0;
static struct class *dev_class;
static struct cdev myalu_cdev;

static void __iomem *r0;
static void __iomem *r1;
static void __iomem *r2;
static void __iomem *r3;
static void __iomem *r4;
static void __iomem *r5;

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
	u32 ready = ioread32(r1);
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
	return 0;
}

static ssize_t myalu_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	char str[16];
	size_t str_len = 0;
	if (0 == *off)
	{
		u32 result = ioread32(r5);
		u32 carry = ioread32(r1);
		carry = carry & (0x00000001);
		sprintf(str, "%d,%d\n", carry, result);
		str_len = strlen(str);
		if (copy_to_user(buf, str, str_len))
		{
			return -EFAULT;
		}
		*off += str_len;
		return str_len;
	}
	return 0;
}

static ssize_t myalu_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	pr_info("Driver Write Function Called...!!!\n");
	u32 operation;
	u32 operand1;
	u32 operand2;
	sscanf(buf, "%d,%d,%d", &operation, &operand1, &operand2);
	pr_info("operation = %x operand1 = %x operand2 = %x \n", operation, operand1, operand2);
	iowrite32(0x00000002, r2);
	iowrite32(0x00000002, r3);
	iowrite32(0x00000002, r4);
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
	r0 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG0_OFFSET, 1);
	r1 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG1_OFFSET, 1);
	r2 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG2_OFFSET, 1);
	r3 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG3_OFFSET, 1);
	r4 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG4_OFFSET, 1);
	r5 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG5_OFFSET, 1);
	iowrite32(0x00000001, r0);
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
	iowrite32(0x00000002, r0);
	iounmap(r0);
	iounmap(r1);
	iounmap(r2);
	iounmap(r3);
	iounmap(r4);
	iounmap(r5);
}

module_init(myalu_driver_init);
module_exit(myalu_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gonzalo Nahuel Vaca <vacagonzalo@gmail.com>");
MODULE_DESCRIPTION("myalumodule - module to communicate with myalu IPCORE.");
MODULE_VERSION("1:0.0");
