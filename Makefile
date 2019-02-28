obj-m += kbd_module.o 
obj-m += logdrv.o
CXXFLAGS = -std=c90
BUILD = -C /lib/modules/$(shell uname -r)/build M=$(shell pwd)

all:
	sudo make $(CFLAGS) $(BUILD) modules
	sudo make clean
clean:
	rm -rf .tmp_versions
	rm .kbd_module.*
	rm .logdrv.*
	rm *.o
	rm *.mod.c
	rm *.order
	rm *.symvers
	rm .cache.*

