#obj-m := timer_tasklet.o hello_proc.o
obj-m += tt.o

DEBUG = n
SAVE_TEMPS = n

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

ifeq ($(SAVE_TEMPS),y)
  ST_FLAGS = -save-temps=obj
endif

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DDEBUG_LOG
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(ST_FLAGS)
EXTRA_CFLAGS += $(DEBFLAGS)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
