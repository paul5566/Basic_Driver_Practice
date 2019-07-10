obj-m	:=basic_ch.o
KDIR 	:=/home/paul/Panzer/4.14_linux-imx
# The KDIR must have to assign a ./direction
modules:
	$(MAKE) -C $(KDIR) SUBDIRS=$(shell pwd) modules
clean:
	rm -f modules.order Module.symvers  *.o *.ko *.mod.c
