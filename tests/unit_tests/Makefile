CC = gcc -g 
#CC = g++ -g 
CFLAGES = -I ./ -I ./include -I ../../include -O2
LIBS = -L ./lib -lcunit -L ../../lib -lks3sdk -lcurl
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
ORI_TARGET := $(patsubst %.o, %, $(OBJS))
TARGET := $(filter-out load_key, $(ORI_TARGET))

RM = rm -rf

all:$(TARGET)

$(TARGET):%:%.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGES) $< -o $@
clean:
	-$(RM) $(OBJS) $(TARGET)
