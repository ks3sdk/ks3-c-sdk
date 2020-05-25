####################64Bit Mode####################
ifeq ($(shell uname -m),x86_64)
CC=gcc
CXX=g++
CXXFLAGS=-g \
  -std=c++11 \
  -pipe \
  -W \
  -O2 \
  -Wall \
  -fPIC
CPPFLAGS=-D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -D__VERSION_ID__=\"1.0.0.0\" \
  -D__BUILD_HOST__=\"`whoami`@`hostname`\"

ifeq ($(release), __UNIT_TEST__)
	CXXFLAGS+=--coverage
	CPPFLAGS+=-D__UNIT_TEST__
	LDFLAGS=-lgcov
endif

ifeq ($(mode), address)
CXXFLAGS+=-fsanitize=address
endif

ifeq ($(mode), thread)
CXXFLAGS+=-fsanitize=thread
endif

INCPATH=-I./
DEPINCPATH=-I./ \
  		   -I./curl \
  		   -I./rapidjson/include

SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc, %.o, ${SRCS})

.PHONY:all
all:libks3.a
	@echo "[[1;32;40mLANGSHAN:BUILD[0m][Target:'[1;32;40m$@[0m']"
	@echo "make all done"

.PHONY:clean
clean:
	@echo "[[1;32;40mLANGSHAN:BUILD[0m][Target:'[1;32;40m$@[0m']"
	rm -rf $(OBJS)
	rm -rf libks3.a
#	rm -rf *.gcno
#	rm -rf *.gcda
#	rm -rf *.gcov

libks3.a:$(OBJS)
	@echo "[[1;32;40mLANGSHAN:BUILD[0m][Target:'[1;32;40m$@[0m']"
	ar crs libks3.a $(OBJS)

%.o : %.cc
	@echo "[[1;32;40mLANGSHAN:BUILD[0m][Target:'[1;32;40m$@[0m']"
	$(CXX) -c $(INCPATH) $(DEPINCPATH) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

endif #ifeq ($(shell uname -m),x86_64)
