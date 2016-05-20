CC = gcc -g 
#CC = g++ -g 

CFLAGES = -I ../include -O2 
LIBS = -L ../lib -lks3sdk -lcurl

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, ${SRCS})
LIBOBJS := $(filter-out main.o , $(OBJS))

TARGET = delete_bucket 
RM = rm -rf

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGES) $(LIBS)
$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGES) $< -o $@
clean:
	-$(RM) $(OBJS) $(TARGET)
