# Module 5: Kernel Modules & Device Drivers

⚠️ **Use a VM! Kernel bugs can crash your system!**

## 📁 Contents

- `LECTURE_NOTES.md` - Kernel programming guide
- `examples/hello_module.c` - Basic kernel module
- `examples/char_device.c` - Character device driver
- `examples/Makefile` - Kernel module build system

## 🎯 Topics

- Kernel space vs user space
- Loadable Kernel Modules (LKM)
- Character device drivers
- /proc interface
- Kernel synchronization
- Debugging techniques

## 🚀 Quick Start

```bash
cd examples/
make
sudo insmod hello_module.ko
dmesg | tail
sudo rmmod hello_module
```

## ⚠️ Requirements

```bash
sudo apt install linux-headers-$(uname -r)
sudo apt install build-essential
```

## ✅ Ready for Weeks 9-10!

