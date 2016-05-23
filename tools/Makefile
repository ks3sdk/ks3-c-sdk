CC = gcc -g 
#CC = g++ -g 

CFLAGES = -I ../include -O2 
LIBS = -L ../lib -lks3sdk -lcurl

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, ${SRCS})
LIBOBJS := $(filter-out main.o , $(OBJS))
TARGET := $(patsubst %.c, %, ${SRCS})

RM = rm -rf

all:$(TARGET)

$(TARGET):%:%.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGES) $< -o $@
clean:
	-$(RM) $(OBJS) $(TARGET)
