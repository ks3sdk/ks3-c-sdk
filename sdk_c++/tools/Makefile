CC = g++ -g -std=c++11 

CFLAGES = -I ../ -O2 
LIBS = -L ../ -lks3 -L ../lib -lcurl -lxml2

SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc, %.o, ${SRCS})
LIBOBJS := $(filter-out main.o , $(OBJS))
TARGET := $(patsubst %.cc, %, ${SRCS})

RM = rm -rf

all:$(TARGET)

$(TARGET):%:%.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
$(OBJS):%.o:%.cc
	$(CC) -c $(CFLAGES) $< -o $@
clean:
	-$(RM) $(OBJS) $(TARGET)
