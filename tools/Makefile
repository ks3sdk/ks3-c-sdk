CC = gcc -g 
#CC = g++ -g 

CFLAGES = -I ../ks3sdk_c -O2 
LIBS = -L ../ks3sdk_c -lks3sdk -L ../ks3sdk_c/lib -lcurl

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
