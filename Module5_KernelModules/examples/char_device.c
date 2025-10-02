/*
 * char_device.c - Simple character device
 * Creates /dev/mychardev that can be read/written
 *
 * Build: make
 * Load: sudo insmod char_device.ko
 * Create device: sudo mknod /dev/mychardev c MAJOR 0
 * Test: echo "Hello" > /dev/mychardev
 *       cat /dev/mychardev
 * Unload: sudo rmmod char_device
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple character device driver");

#define DEVICE_NAME "mychardev"
#define BUFFER_SIZE 1024

static int major_number;
static char device_buffer[BUFFER_SIZE];
static size_t buffer_len = 0;

static int device_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "mychardev: Device opened\n");
  return 0;
}

static int device_release(struct inode *inode, struct file *file) {
  printk(KERN_INFO "mychardev: Device closed\n");
  return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buf,
                           size_t count, loff_t *offset) {
  size_t to_read;

  if (*offset >= buffer_len)
    return 0;

  to_read = min(count, buffer_len - (size_t)*offset);

  if (copy_to_user(user_buf, device_buffer + *offset, to_read))
    return -EFAULT;

  *offset += to_read;
  printk(KERN_INFO "mychardev: Read %zu bytes\n", to_read);

  return to_read;
}

static ssize_t device_write(struct file *file, const char __user *user_buf,
                            size_t count, loff_t *offset) {
  size_t to_write = min(count, (size_t)BUFFER_SIZE);

  if (copy_from_user(device_buffer, user_buf, to_write))
    return -EFAULT;

  buffer_len = to_write;
  printk(KERN_INFO "mychardev: Wrote %zu bytes\n", to_write);

  return to_write;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init chardev_init(void) {
  major_number = register_chrdev(0, DEVICE_NAME, &fops);

  if (major_number < 0) {
    printk(KERN_ALERT "mychardev: Failed to register\n");
    return major_number;
  }

  printk(KERN_INFO "mychardev: Registered with major number %d\n",
         major_number);
  printk(KERN_INFO "Create device: mknod /dev/%s c %d 0\n", DEVICE_NAME,
         major_number);

  return 0;
}

static void __exit chardev_exit(void) {
  unregister_chrdev(major_number, DEVICE_NAME);
  printk(KERN_INFO "mychardev: Unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
