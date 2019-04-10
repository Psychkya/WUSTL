export KERNEL=kernel7

KERNEL_SOURCE=../linux

obj-m := rt_mod_stu8.o

all:
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) modules
