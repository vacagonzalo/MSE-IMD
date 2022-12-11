#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "myalumodule.h"

#define MYALU_S00_AXI_SLV_BASE_ADDR 0x43C00000

#define MYALU_S00_AXI_SLV_REG0_OFFSET 0U
#define MYALU_S00_AXI_SLV_REG1_OFFSET 4U
#define MYALU_S00_AXI_SLV_REG2_OFFSET 8U
#define MYALU_S00_AXI_SLV_REG3_OFFSET 12U
#define MYALU_S00_AXI_SLV_REG4_OFFSET 16U
#define MYALU_S00_AXI_SLV_REG5_OFFSET 20U
#define MYALU_S00_AXI_SLV_REG_SIZE 1U

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

static long int myalu_ioctl(struct file *file, unsigned cmd, unsigned long arg);

// static void axi_little_write(u32 addr, u32 data);
// static u32 axi_little_read(u32 addr);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = myalu_read,
	.write = myalu_write,
	.open = myalu_open,
	.release = myalu_release,
	.unlocked_ioctl = myalu_ioctl,
};

static int myalu_open(struct inode *inode, struct file *file)
{
	u32 ready = ioread32(r1);
	ready = ready >> 1;
	if (0x00000000 == ready)
	{
		pr_info("myalu IPCORE is bussy, are you in QEMU?.\n");
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

static u32 write_operation;
static u32 write_operand1;
static u32 write_operand2;

static ssize_t myalu_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	char str[32];
	if (copy_from_user(str, buf, len))
	{
		return -EFAULT;
	}
	sscanf(str, "%d,%d,%d", &write_operation, &write_operand1, &write_operand2);
	iowrite32(write_operation, r2);
	iowrite32(write_operand1, r3);
	iowrite32(write_operand2, r4);
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
	r0 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG0_OFFSET, MYALU_S00_AXI_SLV_REG_SIZE);
	r1 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG1_OFFSET, MYALU_S00_AXI_SLV_REG_SIZE);
	r2 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG2_OFFSET, MYALU_S00_AXI_SLV_REG_SIZE);
	r3 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG3_OFFSET, MYALU_S00_AXI_SLV_REG_SIZE);
	r4 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG4_OFFSET, MYALU_S00_AXI_SLV_REG_SIZE);
	r5 = ioremap(MYALU_S00_AXI_SLV_BASE_ADDR + MYALU_S00_AXI_SLV_REG5_OFFSET, MYALU_S00_AXI_SLV_REG_SIZE);
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

/* IOCTL *********************************************************************/

static struct compute_t compute = {
	.operation = OPERATION_INVALID,
	.operand1 = 0,
	.operand2 = 0};

static struct result_t result = {
	.value = 0,
	.carry = 0};

static long int myalu_ioctl(struct file *file, unsigned cmd, unsigned long arg)
{
	switch (cmd)
	{
	case WR_VALUE:
		if (copy_from_user(&compute, (struct compute_t *)arg, sizeof(struct compute_t)))
		{
			return -EFAULT;
		}
		iowrite32(compute.operation, r2);
		iowrite32(compute.operand1, r3);
		iowrite32(compute.operand2, r4);
		break;

	case RD_VALUE:
		result.value = ioread32(r5);
		result.carry = ioread32(r1) & (0x00000001);
		if (copy_to_user((struct result_t *)arg, &result, sizeof(struct result_t)))
		{
			return -EFAULT;
		}
		break;

	default:
		break;
	}
	return 0;
}
/*****************************************************************************/
module_init(myalu_driver_init);
module_exit(myalu_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gonzalo Nahuel Vaca <vacagonzalo@gmail.com>");
MODULE_DESCRIPTION("myalumodule - module to communicate with myalu IPCORE.");
MODULE_VERSION("1:0.0");
