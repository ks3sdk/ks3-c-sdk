CC = gcc -g 
#CC = g++ -g 
CFLAGES = -I ./ -I ./include -I ../../include -O2
LIBS = -L ./lib -lcunit -L ../../lib -lks3sdk -lcurl
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, ${SRCS})

TARGET = list_object delete_bucket create_bucket upload_obj \
		 list_bucket delete_object example copy_object upload_file_obj \
		 list_multipart_uploads
RM = rm -rf

all:$(TARGET)

$(TARGET):%:%.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGES) $< -o $@
clean:
	-$(RM) $(OBJS) $(TARGET)
