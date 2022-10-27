CFLAGS = -g -Wall -DDEBUG 
LDFLAGS = -g -Wall -DDEBUG

all: server client

server: server.o server_util.o
	gcc ${LDFLAGS} server.o server_util.o -o server

server.o: server.c server_util.h
	gcc ${CFLAGS} -c server.c -o server.o

server_util.o : server_util.c
	gcc ${CFLAGS} -c server_util.c -o server_util.o

client: client.o client_util.o
	gcc ${LDFLAGS} client.o client_util.o -o client

client.o: client.c client_util.h
	gcc ${CFLAGS} -c client.c -o client.o

client_util.o : client_util.c
	gcc ${CFLAGS} -c client_util.c -o client_util.o

clean: 
	\rm -rf *.o *~ server client