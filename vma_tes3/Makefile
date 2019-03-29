obj-m	:= test_mmap_k.o
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

all:
		$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
