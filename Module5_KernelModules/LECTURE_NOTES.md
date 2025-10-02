# Module 5: Kernel Modules & Device Drivers
## Lecture Notes

⚠️ **WARNING:** Kernel programming can crash your system. Always use a virtual machine!

---

## 📚 Table of Contents
1. [Introduction to Kernel Space](#introduction)
2. [Loadable Kernel Modules (LKM)](#lkm)
3. [Character Device Drivers](#char-devices)
4. [/proc Interface](#proc-interface)
5. [Kernel Synchronization](#synchronization)
6. [Debugging Kernel Code](#debugging)

---

## 1. Introduction to Kernel Space {#introduction}

### Kernel vs User Space

```
┌─────────────────────────────────────┐
│        USER SPACE (Ring 3)          │
│  - Applications                     │
│  - Libraries (libc)                 │
│  - Can be terminated                │
│  - Memory protected                 │
├─────────────────────────────────────┤ ← System call boundary
│       KERNEL SPACE (Ring 0)         │
│  - Kernel core                      │
│  - Device drivers                   │
│  - Kernel modules                   │
│  - Full hardware access             │
│  - Bug = system crash!              │
└─────────────────────────────────────┘
```

### Key Differences

| Aspect | User Space | Kernel Space |
|--------|------------|--------------|
| **Privileges** | Limited | Full hardware access |
| **Memory** | Virtual, protected | Direct physical access |
| **Floating Point** | Allowed | Avoid! (FPU state issues) |
| **Scheduling** | Can sleep | Context-dependent |
| **Stack** | Large (8MB) | Small (4-16KB) |
| **Crash Impact** | Process dies | System panics |

### Why Kernel Modules?

**Advantages:**
- Extend kernel without recompilation
- Load/unload dynamically
- Smaller kernel image
- Easier development

**Use Cases:**
- Device drivers
- Filesystem support
- Network protocols
- System call hooks

---

## 2. Loadable Kernel Modules (LKM) {#lkm}

### Basic Module Structure

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple kernel module");
MODULE_VERSION("1.0");

static int __init hello_init(void) {
    printk(KERN_INFO "Hello, kernel!\n");
    return 0;  // 0 = success
}

static void __exit hello_exit(void) {
    printk(KERN_INFO "Goodbye, kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
```

### Kernel Module Makefile

```makefile
obj-m += hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

### Building and Loading

```bash
# Build
make

# Load module
sudo insmod hello.ko

# Check it's loaded
lsmod | grep hello

# View kernel log
dmesg | tail

# Unload module
sudo rmmod hello

# Module info
modinfo hello.ko
```

### Module Parameters

```c
static int value = 0;
module_param(value, int, 0644);
MODULE_PARM_DESC(value, "An integer parameter");

// Load with parameter:
// sudo insmod hello.ko value=42
```

### Module Dependencies

```c
// Declare dependency on another module
MODULE_DEPEND(my_module, dependency_module, 1, 1, 1);

// Symbol export (for other modules to use)
EXPORT_SYMBOL(my_function);
EXPORT_SYMBOL_GPL(my_gpl_function);  // GPL-only
```

---

## 3. Character Device Drivers {#char-devices}

### Device Types

**Character Device:**
- Access data as byte stream
- Examples: /dev/null, /dev/random, terminals
- Our focus!

**Block Device:**
- Access data in blocks
- Examples: hard drives, USB sticks

**Network Device:**
- Send/receive packets
- Examples: eth0, wlan0

### Character Device Structure

```c
#include <linux/cdev.h>
#include <linux/fs.h>

static int major_number;
static struct cdev my_cdev;

static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *buf,
                           size_t len, loff_t *offset);
static ssize_t device_write(struct file *file, const char __user *buf,
                            size_t len, loff_t *offset);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};
```

### Registering a Character Device

```c
static int __init my_init(void) {
    // Allocate major number
    major_number = register_chrdev(0, "my_device", &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major_number;
    }
    
    printk(KERN_INFO "Registered device with major %d\n", major_number);
    
    // Create device file: mknod /dev/mydev c MAJOR 0
    return 0;
}

static void __exit my_exit(void) {
    unregister_chrdev(major_number, "my_device");
    printk(KERN_INFO "Device unregistered\n");
}
```

### Device Operations

#### open()
```c
static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    
    // Increment usage count
    try_module_get(THIS_MODULE);
    
    return 0;
}
```

#### read()
```c
static ssize_t device_read(struct file *file, char __user *buf,
                           size_t len, loff_t *offset) {
    char kernel_buf[] = "Hello from kernel!";
    size_t to_copy = min(len, sizeof(kernel_buf) - *offset);
    
    if (*offset >= sizeof(kernel_buf))
        return 0;  // EOF
    
    // Copy to user space
    if (copy_to_user(buf, kernel_buf + *offset, to_copy))
        return -EFAULT;
    
    *offset += to_copy;
    return to_copy;
}
```

#### write()
```c
static ssize_t device_write(struct file *file, const char __user *buf,
                            size_t len, loff_t *offset) {
    char kernel_buf[256];
    size_t to_copy = min(len, sizeof(kernel_buf) - 1);
    
    // Copy from user space
    if (copy_from_user(kernel_buf, buf, to_copy))
        return -EFAULT;
    
    kernel_buf[to_copy] = '\0';
    printk(KERN_INFO "Received: %s\n", kernel_buf);
    
    return to_copy;
}
```

#### release()
```c
static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    
    // Decrement usage count
    module_put(THIS_MODULE);
    
    return 0;
}
```

### Creating Device File

**Manual method:**
```bash
# Find major number
cat /proc/devices | grep my_device

# Create device file
sudo mknod /dev/mydev c MAJOR 0
sudo chmod 666 /dev/mydev
```

**Automatic with udev:**
```c
#include <linux/device.h>

static struct class *dev_class;
static struct device *char_device;

// In init:
dev_class = class_create(THIS_MODULE, "my_class");
char_device = device_create(dev_class, NULL, 
                            MKDEV(major_number, 0),
                            NULL, "mydev");

// In exit:
device_destroy(dev_class, MKDEV(major_number, 0));
class_destroy(dev_class);
```

---

## 4. /proc Interface {#proc-interface}

### Creating /proc Entry

```c
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static struct proc_dir_entry *proc_entry;

// Show function
static int my_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "Hello from /proc!\n");
    seq_printf(m, "Some value: %d\n", some_value);
    return 0;
}

// Open function
static int my_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, my_proc_show, NULL);
}

// File operations
static const struct proc_ops my_proc_ops = {
    .proc_open = my_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// In module init:
proc_entry = proc_create("my_proc", 0444, NULL, &my_proc_ops);

// In module exit:
proc_remove(proc_entry);
```

### Reading /proc File

```bash
cat /proc/my_proc
```

### Writable /proc Entry

```c
static ssize_t my_proc_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *pos) {
    char kernel_buf[100];
    
    if (count > sizeof(kernel_buf) - 1)
        count = sizeof(kernel_buf) - 1;
    
    if (copy_from_user(kernel_buf, buf, count))
        return -EFAULT;
    
    kernel_buf[count] = '\0';
    
    // Process input
    sscanf(kernel_buf, "%d", &some_value);
    
    return count;
}

static const struct proc_ops my_proc_ops = {
    .proc_open = my_proc_open,
    .proc_read = seq_read,
    .proc_write = my_proc_write,  // Add write operation
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};
```

---

## 5. Kernel Synchronization {#synchronization}

### Why Synchronization?

**Kernel is preemptible:**
- Interrupts can occur anytime
- Multiple CPUs run simultaneously
- Race conditions possible

### Spinlocks

**Use when:** Cannot sleep, short critical sections

```c
#include <linux/spinlock.h>

static DEFINE_SPINLOCK(my_lock);

// Acquire lock
spin_lock(&my_lock);
// Critical section
spin_unlock(&my_lock);

// IRQ-safe version
unsigned long flags;
spin_lock_irqsave(&my_lock, flags);
// Critical section
spin_unlock_irqrestore(&my_lock, flags);
```

### Mutexes

**Use when:** Can sleep, longer critical sections

```c
#include <linux/mutex.h>

static DEFINE_MUTEX(my_mutex);

// Acquire mutex
mutex_lock(&my_mutex);
// Critical section (can sleep here)
mutex_unlock(&my_mutex);

// Interruptible version
if (mutex_lock_interruptible(&my_mutex))
    return -ERESTARTSYS;
```

### Semaphores

```c
#include <linux/semaphore.h>

static DEFINE_SEMAPHORE(my_sem);  // Binary semaphore

// Down (decrement)
down(&my_sem);
// Critical section
up(&my_sem);

// Interruptible
if (down_interruptible(&my_sem))
    return -ERESTARTSYS;
```

### Atomic Operations

```c
#include <linux/atomic.h>

atomic_t counter = ATOMIC_INIT(0);

atomic_inc(&counter);
atomic_dec(&counter);
int value = atomic_read(&counter);
atomic_set(&counter, 10);

// Atomic compare and swap
if (atomic_cmpxchg(&counter, old, new) == old) {
    // Successfully changed
}
```

### Read-Write Locks

```c
#include <linux/rwlock.h>

static DEFINE_RWLOCK(my_rwlock);

// Multiple readers
read_lock(&my_rwlock);
// Read data
read_unlock(&my_rwlock);

// Single writer
write_lock(&my_rwlock);
// Write data
write_unlock(&my_rwlock);
```

---

## 6. Debugging Kernel Code {#debugging}

### printk()

```c
// Priority levels (increasing severity)
printk(KERN_DEBUG "Debug message\n");
printk(KERN_INFO "Informational\n");
printk(KERN_NOTICE "Normal but significant\n");
printk(KERN_WARNING "Warning\n");
printk(KERN_ERR "Error\n");
printk(KERN_CRIT "Critical\n");
printk(KERN_ALERT "Action must be taken\n");
printk(KERN_EMERG "System is unusable\n");

// View messages
dmesg
dmesg | tail -20
```

### Dynamic Debug

```c
// Enabled only when CONFIG_DYNAMIC_DEBUG is set
pr_debug("Debug info: %d\n", value);

// Enable at runtime
echo 'file mymodule.c +p' > /sys/kernel/debug/dynamic_debug/control
```

### /proc and /sys for Debugging

```c
// Expose debug info via /proc
seq_printf(m, "Debug: ptr=%p, value=%d\n", ptr, value);
```

### Kernel Oops Analysis

When kernel crashes, it prints an "oops" message:

```
BUG: unable to handle kernel NULL pointer dereference at 0000000000000008
IP: [<ffffffffa0123456>] my_function+0x12/0x34 [my_module]
Call Trace:
 [<ffffffffa0123789>] another_function+0x45/0x67 [my_module]
```

**Decode:**
```bash
addr2line -e my_module.ko 0x12
# Shows source file and line number
```

### Common Mistakes

```c
// ❌ BAD: User space functions
printf("Debug\n");          // WRONG! Use printk()
malloc(100);                // WRONG! Use kmalloc()

// ❌ BAD: Floating point
float x = 1.5;              // WRONG! Avoid in kernel

// ❌ BAD: Large stack usage
char buffer[4096];          // Too big! Kernel stack is small

// ✅ GOOD: Kernel equivalents
printk(KERN_INFO "Debug\n");
void *ptr = kmalloc(100, GFP_KERNEL);
char *buffer = kmalloc(4096, GFP_KERNEL);
```

---

## 📝 Summary

### Key Concepts

1. **Kernel space** has full privileges but crashes affect entire system
2. **Loadable modules** extend kernel without recompilation
3. **Character devices** provide file-like interface
4. **/proc interface** exposes kernel data to user space
5. **Synchronization** required for concurrent access
6. **Debugging** uses printk() and dmesg

### Safety Rules

⚠️ **Always:**
- Use a VM for kernel development
- Check return values
- Use proper synchronization
- Test incrementally
- Keep backups

⚠️ **Never:**
- Use floating point
- Assume large stack
- Call user-space functions
- Ignore errors

### Useful Commands

```bash
# Module management
lsmod                    # List loaded modules
modinfo module.ko        # Show module info
insmod module.ko         # Load module
rmmod module             # Unload module
modprobe module          # Load with dependencies

# Debugging
dmesg                    # Kernel messages
journalctl -k            # Kernel log
cat /proc/devices        # List devices
cat /proc/modules        # List modules
```

---

**This completes the course! You're now ready to write kernel-level code!** 🐧⚙️

