obj-m	:=gpio_hello.o
KDIR 	:=/home/paul/Panzer/linux-imx_4_9
MYCC		=/home/paul/opt/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-

# The KDIR must have to assign a ./direction
modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(MYCC) -C $(KDIR) SUBDIRS=$(shell pwd) modules
clean:
	rm -f modules.order Module.symvers  *.o *.ko *.mod.c
