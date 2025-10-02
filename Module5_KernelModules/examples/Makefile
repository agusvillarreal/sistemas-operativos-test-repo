# Kernel Module Makefile

obj-m += hello_module.o
obj-m += char_device.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	@echo "✓ Kernel modules built"
	@echo "Load with: sudo insmod hello_module.ko"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	@echo "✓ Clean complete"

load:
	@echo "Loading hello_module..."
	sudo insmod hello_module.ko
	dmesg | tail -5

unload:
	@echo "Unloading hello_module..."
	sudo rmmod hello_module
	dmesg | tail -5

.PHONY: all clean load unload

