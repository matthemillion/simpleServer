CC = gcc
CFLAGS = -std=c99 -c

all: server

server: Net.o ThreadPool.o server.o
	$(CC) Net.o ThreadPool.o server.o -pthread -o server

Net.o: Net.c
	$(CC) $(CFLAGS) Net.c

ThreadPool.o: ThreadPool.c
	$(CC) -pthread $(CFLAGS) ThreadPool.c

server.o: server.c
	$(CC) $(CFLAGS) server.c

clean:
	rm *o server
