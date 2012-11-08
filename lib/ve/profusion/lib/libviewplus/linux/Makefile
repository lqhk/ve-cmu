ifneq ($(KERNELRELEASE), )
	obj-m := VP1066.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -rf VP1066.o VP1066.ko VP1066.mod.* modules.order Module.symvers .tmp_versions .VP1066.*.cmd
