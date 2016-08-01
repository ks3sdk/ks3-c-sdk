CC = gcc -g 
#CC = g++ -g 
CFLAGES = -I ./curl# -O2
LIBS = -L ./lib -lcurl
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, ${SRCS})
LIBOBJS := $(filter-out example.o , $(OBJS))

TARGET = ks3demo
RM = rm -rf

OUTPUT := ./libks3sdk.a

#all:$(TARGET)
all:$(OUTPUT)

install:
	mkdir -p ../lib/
	mkdir -p ../include/
	cp api.h buffer.h ../include/
	cp libks3sdk.a ../lib/
	cp ./lib/* ../lib/

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGES) $(LIBS)
./libks3sdk.a : $(LIBOBJS)
	$(AR) rcs  $@ $^
$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGES) $< -o $@
clean:
	-$(RM) $(OUTPUT) $(OBJS) $(TARGET) ../include/ ../lib/
