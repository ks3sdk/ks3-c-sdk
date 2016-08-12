#CC = gcc -g 
CC = g++ -g 
CFLAGES = -I ../../ks3sdk_c -I ./pandora/output/include/ #-O2
LIBS = -L ../../ks3sdk_c -lks3sdk -L ../../ks3sdk_c/lib -lcurl -L ./pandora/output/lib/ -lpandora
SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc, %.o, ${SRCS})

TARGET = press_entry
RM = rm -rf

all:$(TARGET)

$(TARGET):%:$(OBJS)
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)

%.o: %.cc
	$(CC) -c $(CFLAGES) $< -o $@

clean:
	-$(RM) $(OBJS) $(TARGET) *.o
