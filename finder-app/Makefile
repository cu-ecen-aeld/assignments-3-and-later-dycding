

CC := $(CROSS_COMPILE)gcc

default: writer

writer: writer.o
	$(CC) -o writer writer.c

clean:
	-rm -f writer *.o
