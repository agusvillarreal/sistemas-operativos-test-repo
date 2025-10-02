/*
 * hello_module.c - Simple kernel module
 *
 * Build: make
 * Load: sudo insmod hello_module.ko
 * Check: dmesg | tail
 * Unload: sudo rmmod hello_module
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Hello World kernel module");
MODULE_VERSION("1.0");

static int value = 0;
module_param(value, int, 0644);
MODULE_PARM_DESC(value, "An integer parameter (default=0)");

static int __init hello_init(void) {
  printk(KERN_INFO "Hello, kernel! Parameter value=%d\n", value);
  return 0;
}

static void __exit hello_exit(void) { printk(KERN_INFO "Goodbye, kernel!\n"); }

module_init(hello_init);
module_exit(hello_exit);
