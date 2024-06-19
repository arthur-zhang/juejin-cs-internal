KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build

obj-m += helloworld.o

all:
	make -C $(KERNEL_SOURCE) M=$(PWD) modules

clean:
	make -C $(KERNEL_SOURCE) M=$(PWD) clean