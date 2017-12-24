PWD := $(shell pwd)
KVERSION := $(shell uname -r)

//TODO: need to assign the kernel source path
KERNEL_DIR = ../


obj-m += mydrive.o
all:
	make -C  $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean

