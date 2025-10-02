# Homework 5: Custom Kernel Module
## Standalone Assignment - Loadable Kernel Modules & Device Drivers

**Due Date:** 3 weeks from assignment date  
**Weight:** 25% of final grade  
**Points:** 100 points  
**Submission:** Source code + README + Makefile + demo video + kernel logs

⚠️ **WARNING:** Kernel programming can crash your system. Use a VM!

---

## 📌 Assignment Overview

This is **Homework 5 of 5** in the Linux Systems Programming course. Each homework is an **independent project**:

- Homework 1: Basic Process Management
- Homework 2: Zombie Process Handler
- Homework 3: Scheduler Simulator with GUI
- Homework 4: Chat System using IPC
- **Homework 5** (This): Custom Kernel Module ← You are here

---

## 🎯 Learning Objectives

By completing this homework, you will:
1. Understand kernel vs user space programming
2. Implement a loadable kernel module (LKM)
3. Create a character device driver
4. Use /proc and /sys filesystems
5. Handle kernel synchronization primitives
6. Debug kernel code safely

---

## 📋 Assignment Description

You will implement a **Character Device Driver** that acts as a simple message buffer:
- Loadable kernel module (insert/remove with insmod/rmmod)
- Character device that appears as /dev/msgbuf
- Read/write operations from user space
- /proc interface for statistics
- Proper synchronization for concurrent access
- Clean module insertion/removal

---

## 🔧 Requirements

### Part 1: Basic Kernel Module (15 points)

Create `msgbuf.c` with module infrastructure:

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Message Buffer Character Device");
MODULE_VERSION("1.0");

static int __init msgbuf_init(void) {
    printk(KERN_INFO "msgbuf: Module loaded\n");
    return 0;
}

static void __exit msgbuf_exit(void) {
    printk(KERN_INFO "msgbuf: Module unloaded\n");
}

module_init(msgbuf_init);
module_exit(msgbuf_exit);
```

**Makefile for kernel module:**
```makefile
obj-m += msgbuf.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

**Test:**
```bash
make
sudo insmod msgbuf.ko
dmesg | tail
sudo rmmod msgbuf
dmesg | tail
```

---

### Part 2: Character Device (30 points)

Implement character device operations:

```c
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "msgbuf"
#define BUFFER_SIZE 4096

static int major_number;
static struct cdev msgbuf_cdev;
static char device_buffer[BUFFER_SIZE];
static size_t buffer_len = 0;

// Device operations
static int msgbuf_open(struct inode *inode, struct file *file);
static int msgbuf_release(struct inode *inode, struct file *file);
static ssize_t msgbuf_read(struct file *file, char __user *user_buf,
                           size_t count, loff_t *offset);
static ssize_t msgbuf_write(struct file *file, const char __user *user_buf,
                            size_t count, loff_t *offset);
static long msgbuf_ioctl(struct file *file, unsigned int cmd, 
                        unsigned long arg);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = msgbuf_open,
    .release = msgbuf_release,
    .read = msgbuf_read,
    .write = msgbuf_write,
    .unlocked_ioctl = msgbuf_ioctl,
};
```

**Required Operations:**

#### open()
```c
static int msgbuf_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "msgbuf: Device opened\n");
    // Allow multiple opens (read/write)
    return 0;
}
```

#### read()
```c
static ssize_t msgbuf_read(struct file *file, char __user *user_buf,
                           size_t count, loff_t *offset) {
    // Copy data from kernel buffer to user space
    // Handle offset and partial reads
    // Update statistics
    
    if (*offset >= buffer_len) {
        return 0;  // EOF
    }
    
    size_t to_read = min(count, buffer_len - *offset);
    
    if (copy_to_user(user_buf, device_buffer + *offset, to_read)) {
        return -EFAULT;
    }
    
    *offset += to_read;
    return to_read;
}
```

#### write()
```c
static ssize_t msgbuf_write(struct file *file, const char __user *user_buf,
                            size_t count, loff_t *offset) {
    // Copy data from user space to kernel buffer
    // Check buffer overflow
    // Update statistics
    
    if (count > BUFFER_SIZE) {
        count = BUFFER_SIZE;  // Truncate
    }
    
    if (copy_from_user(device_buffer, user_buf, count)) {
        return -EFAULT;
    }
    
    buffer_len = count;
    device_buffer[buffer_len] = '\0';
    
    printk(KERN_INFO "msgbuf: Wrote %zu bytes\n", count);
    return count;
}
```

#### ioctl() - Custom Commands
```c
#define MSGBUF_IOCTL_CLEAR   _IO('M', 1)
#define MSGBUF_IOCTL_GETSIZE _IOR('M', 2, int)
#define MSGBUF_IOCTL_RESIZE  _IOW('M', 3, int)

static long msgbuf_ioctl(struct file *file, unsigned int cmd, 
                        unsigned long arg) {
    switch (cmd) {
    case MSGBUF_IOCTL_CLEAR:
        buffer_len = 0;
        memset(device_buffer, 0, BUFFER_SIZE);
        return 0;
        
    case MSGBUF_IOCTL_GETSIZE:
        return buffer_len;
        
    case MSGBUF_IOCTL_RESIZE:
        // Not implemented for fixed buffer
        return -EINVAL;
        
    default:
        return -EINVAL;
    }
}
```

---

### Part 3: /proc Interface (20 points)

Create `/proc/msgbuf_stats`:

```c
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static struct proc_dir_entry *proc_entry;

// Statistics
static struct {
    unsigned long opens;
    unsigned long reads;
    unsigned long writes;
    unsigned long bytes_read;
    unsigned long bytes_written;
} stats;

static int msgbuf_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "Message Buffer Statistics\n");
    seq_printf(m, "=========================\n");
    seq_printf(m, "Opens:          %lu\n", stats.opens);
    seq_printf(m, "Reads:          %lu\n", stats.reads);
    seq_printf(m, "Writes:         %lu\n", stats.writes);
    seq_printf(m, "Bytes Read:     %lu\n", stats.bytes_read);
    seq_printf(m, "Bytes Written:  %lu\n", stats.bytes_written);
    seq_printf(m, "Buffer Size:    %d\n", BUFFER_SIZE);
    seq_printf(m, "Current Length: %zu\n", buffer_len);
    return 0;
}

static int msgbuf_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, msgbuf_proc_show, NULL);
}

static const struct proc_ops msgbuf_proc_ops = {
    .proc_open = msgbuf_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// In module init:
proc_entry = proc_create("msgbuf_stats", 0444, NULL, &msgbuf_proc_ops);
```

**Usage:**
```bash
cat /proc/msgbuf_stats
```

---

### Part 4: Synchronization (20 points)

Implement proper locking for concurrent access:

```c
#include <linux/mutex.h>
#include <linux/spinlock.h>

static DEFINE_MUTEX(buffer_mutex);
static DEFINE_SPINLOCK(stats_lock);

static ssize_t msgbuf_read(struct file *file, char __user *user_buf,
                           size_t count, loff_t *offset) {
    ssize_t ret;
    
    // Lock buffer for reading
    if (mutex_lock_interruptible(&buffer_mutex)) {
        return -ERESTARTSYS;
    }
    
    // ... perform read ...
    
    mutex_unlock(&buffer_mutex);
    
    // Update statistics (use spinlock for counters)
    spin_lock(&stats_lock);
    stats.reads++;
    stats.bytes_read += ret;
    spin_unlock(&stats_lock);
    
    return ret;
}
```

**Requirements:**
- Mutex for buffer access (can sleep)
- Spinlock for statistics (fast, no sleep)
- No deadlocks
- Handle interrupted waits (ERESTARTSYS)

---

### Part 5: User-Space Test Program (15 points)

Create `test_msgbuf.c`:

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/msgbuf"
#define MSGBUF_IOCTL_CLEAR   _IO('M', 1)
#define MSGBUF_IOCTL_GETSIZE _IOR('M', 2, int)

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    // Test write
    const char *msg = "Hello from user space!";
    ssize_t written = write(fd, msg, strlen(msg));
    printf("Wrote %zd bytes\n", written);
    
    // Test read
    char buffer[256] = {0};
    lseek(fd, 0, SEEK_SET);  // Reset to beginning
    ssize_t read_bytes = read(fd, buffer, sizeof(buffer));
    printf("Read %zd bytes: %s\n", read_bytes, buffer);
    
    // Test ioctl
    int size = ioctl(fd, MSGBUF_IOCTL_GETSIZE);
    printf("Current buffer size: %d\n", size);
    
    ioctl(fd, MSGBUF_IOCTL_CLEAR);
    printf("Buffer cleared\n");
    
    close(fd);
    return 0;
}
```

**Multi-Process Test:**
```c
// Fork 10 processes, all writing simultaneously
for (int i = 0; i < 10; i++) {
    if (fork() == 0) {
        // Child writes repeatedly
        int fd = open(DEVICE, O_WRONLY);
        for (int j = 0; j < 100; j++) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Process %d, iteration %d\n", getpid(), j);
            write(fd, buf, strlen(buf));
            usleep(1000);
        }
        close(fd);
        exit(0);
    }
}

// Parent waits
for (int i = 0; i < 10; i++) wait(NULL);
```

---

## 📦 Deliverables

```
homework5/
├── module/
│   ├── msgbuf.c                # Kernel module
│   ├── Makefile                # Kernel Makefile
│   └── msgbuf.h                # ioctl definitions
├── userspace/
│   ├── test_msgbuf.c           # Basic test
│   ├── test_concurrent.c       # Multi-process test
│   ├── test_stress.c           # Stress test
│   └── Makefile                # User programs
├── scripts/
│   ├── load_module.sh          # insmod + create device
│   ├── unload_module.sh        # Remove device + rmmod
│   └── test_all.sh             # Run all tests
├── docs/
│   ├── DESIGN.md               # Design decisions
│   ├── KERNEL_LOGS.txt         # dmesg output
│   ├── TESTING.md              # Test results
│   └── ANALYSIS.pdf            # Performance analysis
├── README.md
└── demo.mp4
```

---

## 📊 Grading Rubric

| Component | Points | Criteria |
|-----------|--------|----------|
| **Part 1: Basic Module** | 15 | |
| - Module loads/unloads | 8 | insmod/rmmod works |
| - Proper logging | 4 | printk messages |
| - Module metadata | 3 | LICENSE, AUTHOR |
| **Part 2: Character Device** | 30 | |
| - Device registration | 8 | /dev/msgbuf created |
| - open/release | 4 | Multiple opens |
| - read() | 8 | Correct data returned |
| - write() | 8 | Data stored properly |
| - ioctl() | 2 | Custom commands |
| **Part 3: /proc Interface** | 20 | |
| - /proc file creation | 8 | Appears in /proc |
| - Statistics tracking | 10 | All metrics |
| - Formatted output | 2 | Readable |
| **Part 4: Synchronization** | 20 | |
| - Mutex usage | 10 | Proper locking |
| - No race conditions | 8 | Concurrent access safe |
| - Deadlock-free | 2 | No hangs |
| **Part 5: Test Program** | 15 | |
| - Basic operations | 8 | Read/write/ioctl |
| - Concurrent test | 5 | Multi-process |
| - Error handling | 2 | Checks returns |
| **Code Quality** | 10 | |
| - Error handling | 3 | Checks all returns |
| - Memory management | 3 | No leaks |
| - Code style | 2 | Linux kernel style |
| - Comments | 2 | Clear docs |
| **Documentation** | 10 | |
| - README | 4 | Complete guide |
| - Design doc | 3 | Explains choices |
| - Demo video | 3 | Shows all features |

**Total: 100 points**

**Bonus:**
- /sys interface (+5 points)
- Dynamic buffer allocation (+10 points)
- Multiple device support (+10 points)

---

## 🧪 Testing

### Load and Test

```bash
# Build module
cd module/
make

# Load module
sudo insmod msgbuf.ko
lsmod | grep msgbuf

# Create device node (if not using udev)
sudo mknod /dev/msgbuf c $(cat /proc/devices | grep msgbuf | awk '{print $1}') 0
sudo chmod 666 /dev/msgbuf

# Test from command line
echo "Hello" | sudo dd of=/dev/msgbuf
sudo dd if=/dev/msgbuf bs=100 count=1

# Check statistics
cat /proc/msgbuf_stats

# Run test program
cd ../userspace/
make
./test_msgbuf

# Check kernel logs
dmesg | tail -20

# Unload module
sudo rmmod msgbuf
```

### Stress Test

```bash
# Concurrent writes (should not crash!)
for i in {1..100}; do
    echo "Process $i" > /dev/msgbuf &
done
wait

# Check for errors
dmesg | grep -i error
```

---

## 💡 Implementation Tips

### Kernel vs User Space

| User Space | Kernel Space |
|------------|--------------|
| `malloc()` | `kmalloc()`, `kzalloc()` |
| `free()` | `kfree()` |
| `printf()` | `printk()` |
| `memcpy()` | `copy_to_user()`, `copy_from_user()` |
| Can sleep | Careful! Some contexts can't sleep |

### Debugging Kernel Code

```bash
# View kernel messages
dmesg
dmesg -w  # Watch mode

# Increase log level
sudo dmesg -n 8

# Use pr_debug (requires DEBUG define)
#define DEBUG
#include <linux/kernel.h>
pr_debug("Debug message\n");

# Check for kernel oops
dmesg | grep -i oops
```

### Common Mistakes

```c
// ❌ BAD: Using user-space functions
printf("Hello\n");           // WRONG!
malloc(100);                 // WRONG!

// ✅ GOOD: Using kernel functions
printk(KERN_INFO "Hello\n"); // Correct
kmalloc(100, GFP_KERNEL);    // Correct

// ❌ BAD: Direct pointer access
char *user_ptr = ...;
*user_ptr = 'A';            // Crash!

// ✅ GOOD: Safe copy
copy_to_user(user_ptr, &data, sizeof(data));
```

---

## ⚠️ Safety Warnings

1. **Use a Virtual Machine!** Kernel bugs can crash the entire system
2. **Save your work** before loading new modules
3. **Test incrementally** - don't write everything at once
4. **Check dmesg frequently** for errors
5. **Keep backups** - kernel panics can corrupt filesystems

---

## 📚 Resources

- Linux Device Drivers (LDD3): https://lwn.net/Kernel/LDD3/
- Kernel Documentation: https://www.kernel.org/doc/html/latest/
- `man 2 ioctl`
- `/usr/src/linux-headers-$(uname -r)/include/linux/`

### Useful Commands

```bash
# Kernel version
uname -r

# Install headers
sudo apt install linux-headers-$(uname -r)

# Module info
modinfo msgbuf.ko

# Module dependencies
lsmod

# Current devices
cat /proc/devices
ls -l /dev/msgbuf

# Kernel log
journalctl -k -f
```

---

## ❓ FAQ

**Q: Module won't load - "Invalid module format"**  
A: Compiled for wrong kernel version. Rebuild with current headers.

**Q: "Operation not permitted"**  
A: Need sudo for insmod/rmmod.

**Q: Can't find /dev/msgbuf**  
A: Create manually with `mknod` or use udev rules.

**Q: System froze after loading module!**  
A: You have a kernel panic. Reboot VM, check code for bugs.

**Q: How do I debug a kernel panic?**  
A: Look at dmesg output for stack trace, check line numbers.

---

**Good luck! Welcome to kernel programming!** 🐧⚙️

**Remember: "With great power comes great responsibility... and occasional kernel panics."**

