CC = gcc                              
CFLAGES = -I curl -O2   
LIBS = -L lib -lcurl
OBJS = api.o buffer.o make_header.o file_deal.o md5.o sign.o util.o main.o rb_tree.o

TARGET = ks3demo
RM = rm -f
$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGES) $(LIBS)
$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGES) $< -o $@
clean:
	-$(RM) $(TARGET) $(OBJS)
