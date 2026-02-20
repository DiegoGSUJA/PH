BINARY := DGDriver

# This line tells the kernel that DGDriver.o is made of these two parts
$(BINARY)-y := DGMain.o DGFops.o

# This tells the kernel to build the module
obj-m := $(BINARY).o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
