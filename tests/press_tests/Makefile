#CC = gcc -g 
CC = g++ -g 
CFLAGES = -I ../../include -I ./pandora/output/include/ -O2
LIBS = -L ../../lib -lks3sdk -lcurl -L ./pandora/output/lib/ -lpandora
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, ${SRCS})

TARGET = press_entry #press_upload #press_download #walk_dir
RM = rm -rf

all:$(TARGET)

press_entry:entry.o press_download.o press_upload.o press_common.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
press_download:entry.o press_download.o press_common.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
press_upload:entry.o press_upload.o press_common.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
entry.o:entry.cc
	$(CC) -c $(CFLAGES) $< -o $@
press_upload.o:press_upload.cc
	$(CC) -c $(CFLAGES) $< -o $@
press_common.o:press_common.cc
	$(CC) -c $(CFLAGES) $< -o $@
press_download.o:press_download.cc
	$(CC) -c $(CFLAGES) $< -o $@

clean:
	-$(RM) $(OBJS) $(TARGET) *.o
