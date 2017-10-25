
CXX?=g++
CXX_FLAGS?=-std=c++11 -Wall -Wextra -Werror -g
CXX_LIBS?=-lboost_system -lrados -lradosstriper
CXX_INC?=$(LOCAL_LIBRADOS_INC)
CXX_CC=$(CXX) $(CXX_FLAGS) $(CXX_INC) $(LOCAL_LIBRADOS)

# Relative path to the Ceph source:
CEPH_SRC_HOME?=/home/sungho/ceph/src
CEPH_BLD_HOME?=/home/sungho/ceph/build

LOCAL_LIBRADOS?=-L$(CEPH_BLD_HOME)/lib/ -Wl,-rpath,$(CEPH_BLD_HOME)/lib
LOCAL_LIBRADOS_INC?=-I$(CEPH_SRC_HOME)/include

all: chara_cpp

# Build against the system librados instead of the one in the build tree:
all-system: LOCAL_LIBRADOS=
all-system: LOCAL_LIBRADOS_INC=
all-system: all

chara_cpp: chara.cc
	$(CXX_CC) -o chara_cpp chara.cc $(CXX_LIBS)

clean:
	rm -f chara_cpp
