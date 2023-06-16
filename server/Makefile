CC := $(CROSS_COMPILE)gcc
CFLAGS=-Werror -Wall -g

all: aesdsocket

default: all;

aesdsocket: aesdsocket.o
	$(CC) $(CFLAGS) -o aesdsocket aesdsocket.o

clean:
	-rm -f aesdsocket *.o
