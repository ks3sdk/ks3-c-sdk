CC = gcc -g 
#CC = g++ -g 
CFLAGES = -I ./ -I ./include -I ../../include -O2
LIBS = -L ./lib -lcunit -L ../../lib -lks3sdk -lcurl
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, ${SRCS})

TARGET = list_object delete_bucket create_bucket upload_obj list_bucket delete_object example copy_object
RM = rm -rf

all:$(TARGET)

list_object:list_object.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
list_object.o:list_object.c
	$(CC) -c $(CFLAGES) $< -o $@
delete_bucket:delete_bucket.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
delete_bucket.o:delete_bucket.c
	$(CC) -c $(CFLAGES) $< -o $@
create_bucket:create_bucket.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
create_bucket.o:create_bucket.c
	$(CC) -c $(CFLAGES) $< -o $@
walk_dir:walk_dir.o
	$(CC) -o $@ $< $(CFLAGES) $(LIBS)
walk_dir.o:walk_dir.c
	$(CC) -c $(CFLAGES) $< -o $@
copy_object:copy_object.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
copy_object.o:copy_object.c
	$(CC) -c $(CFLAGES) $< -o $@
upload_obj:upload_obj.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
upload_obj.o:upload_obj.c
	$(CC) -c $(CFLAGES) $< -o $@
list_bucket:list_bucket.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
list_bucket.o:list_bucket.c
	$(CC) -c $(CFLAGES) $< -o $@
delete_object:delete_object.o load_key.o
	$(CC) -o $@ $^ $(CFLAGES) $(LIBS)
delete_object.o:delete_object.c
	$(CC) -c $(CFLAGES) $< -o $@
example:example.o
	$(CC) -o $@ $< $(CFLAGES) $(LIBS)
example.o:example.c
	$(CC) -c $(CFLAGES) $< -o $@
load_key.o:load_key.c
	$(CC) -c $(CFLAGES) $< -o $@
#$(TARGET):$(OBJS)
#	$(CC) -o $(TARGET) $(OBJS) $(CFLAGES) $(LIBS)
#$(OBJS):%.o:%.c
#	$(CC) -c $(CFLAGES) $< -o $@
clean:
	-$(RM) $(OBJS) $(TARGET)
