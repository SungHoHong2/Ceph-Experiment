SEASTAR ="/home/sungho/seastar"
TARGET = "server"
TARGET_2 = "client"


all:
	g++ -g `pkg-config --cflags --libs ${SEASTAR}/build/release/seastar.pc` ${TARGET}.cc -o ${TARGET}
	g++ -g `pkg-config --cflags --libs ${SEASTAR}/build/release/seastar.pc` ${TARGET_2}.cc -o ${TARGET_2}


.PHONY: clean
clean:
	rm -f *.o ${TARGET} ${TARGET_2}