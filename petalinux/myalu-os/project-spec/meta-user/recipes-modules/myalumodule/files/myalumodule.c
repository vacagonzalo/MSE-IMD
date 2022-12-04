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

dev_t dev = 0;
static struct class *dev_class;
static struct cdev myalu_cdev;

void __iomem *r0 = (void *)0x43C00000;
void __iomem *r1 = (void *)0x43C00004;
void __iomem *r2 = (void *)0x43C00008;
void __iomem *r3 = (void *)0x43C0000C;
void __iomem *r4 = (void *)0x43C00010;
void __iomem *r5 = (void *)0x43C00014;

static int __init myalu_driver_init(void);
static void __exit myalu_driver_exit(void);
static int myalu_open(struct inode *inode, struct file *file);
static int myalu_release(struct inode *inode, struct file *file);
static ssize_t myalu_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t myalu_write(struct file *filp, const char *buf, size_t len, loff_t *off);

static struct file_operations fops =
	{
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

static ssize_t myalu_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	wmb();
	u32 result = ioread32(r5);
	wmb();
	u32 carry = ioread32(r1);
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
}

module_init(myalu_driver_init);
module_exit(myalu_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gonzalo Nahuel Vaca <vacagonzalo@gmail.com>");
MODULE_DESCRIPTION("myalumodule - module to communicate with myalu IPCORE.");
MODULE_VERSION("1:0.0");