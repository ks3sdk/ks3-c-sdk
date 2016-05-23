#CC = gcc -g 
CC = g++ -g 
CFLAGES = -I ../../include -I ./pandora/output/include/ -O2
LIBS = -L ../../lib -lks3sdk -lcurl -L ./pandora/output/lib/ -lpandora
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
