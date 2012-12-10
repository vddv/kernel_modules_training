#obj-m := timer_tasklet.o hello_proc.o
#obj-m += tt.o
#obj-m += irq_m.o
obj-m += threads.o

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
EXTRA_CFLAGS += -std=gnu99

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
