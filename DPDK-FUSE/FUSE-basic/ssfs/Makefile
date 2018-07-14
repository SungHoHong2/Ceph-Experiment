ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk


COMPILER = gcc
FILESYSTEM_FILES = ssfs.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o ssfs `pkg-config fuse --cflags --libs`
	echo 'To Mount: ./ssfs -f [mount point]'

clean:
	rm ssfs






# binary name
# APP = dpdk-server

# all source are stored in SRCS-y
# SRCS-y := main.c

# CFLAGS += -O3

# include $(RTE_SDK)/mk/rte.extapp.mk
