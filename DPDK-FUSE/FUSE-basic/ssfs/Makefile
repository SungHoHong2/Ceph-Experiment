ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

# binary name
APP = ssfs

# all source are stored in SRCS-y
SRCS-y := ssfs.c -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse -lfuse
CFLAGS += -O3
include $(RTE_SDK)/mk/rte.extapp.mk

