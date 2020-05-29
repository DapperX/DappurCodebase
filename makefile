export CXX = g++
export CXXFLAG = -std=c++17 -Wall -Wextra

ifeq ($(MODE),debug)
	CXXFLAG += -g3
else ifeq ($(MODE),release)
	CXXFLAG += -O2 -march=native
endif

export DIR_MAIN = $(shell pwd)
export DIR_LIB = $(DIR_MAIN)
export DIR_INCLUDE = $(DIR_LIB)

CXXFLAG += $(DIR_INCLUDE:%=-I%)

.PHONY : test build_test clean

test :
	$(MAKE) run -C test

build_test :
	$(MAKE) build -C test

clean :
	$(MAKE) clean -C test