SEASTAR ="/home/sungho/seastar"
TARGET ="send"

all:
	g++  `pkg-config --cflags --libs ${SEASTAR}/build/release/seastar.pc` ${TARGET}.cc -o ${TARGET}

.PHONY: clean
clean:
	rm -f *.o ${TARGET}
