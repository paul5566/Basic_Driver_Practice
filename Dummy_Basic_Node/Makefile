obj-m	:=dummy.o
KDIR 	:=/home/paul/Panzer/4_14_linux
# The KDIR must have to assign a ./direction
modules:
	$(MAKE) -C $(KDIR) SUBDIRS=$(shell pwd) modules
clean:
	rm -f modules.order Module.symvers  *.o *.ko *.mod.c
