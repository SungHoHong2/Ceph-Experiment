SEASTAR ="/home/sungho/seastar"
TARGET ="httpd"
TARGET2 = "client"

all:
	g++  `pkg-config --cflags --libs ${SEASTAR}/build/release/seastar.pc` ${TARGET}.cc -o ${TARGET}
	g++  `pkg-config --cflags --libs ${SEASTAR}/build/release/seastar.pc` ${TARGET2}.cc -o ${TARGET2}


.PHONY: clean
clean:
	rm -f *.o ${TARGET} ${TARGET2}
