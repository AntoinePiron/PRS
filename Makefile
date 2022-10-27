CFLAGS = -g -Wall -DDEBUG 
LDFLAGS = -g -Wall -DDEBUG
CLIENT_DIR = client/
SERVER_DIR = server/

all: server client

server: ${SERVER_DIR}server.o ${SERVER_DIR}server_util.o
	gcc ${LDFLAGS} ${SERVER_DIR}server.o ${SERVER_DIR}server_util.o -o s

${SERVER_DIR}server.o: ${SERVER_DIR}server.c ${SERVER_DIR}server_util.h
	gcc ${CFLAGS} -c ${SERVER_DIR}server.c -o ${SERVER_DIR}server.o

${SERVER_DIR}server_util.o : ${SERVER_DIR}server_util.c
	gcc ${CFLAGS} -c ${SERVER_DIR}server_util.c -o ${SERVER_DIR}server_util.o

client: ${CLIENT_DIR}client.o ${CLIENT_DIR}client_util.o
	gcc ${LDFLAGS} ${CLIENT_DIR}client.o ${CLIENT_DIR}client_util.o -o c

${CLIENT_DIR}client.o: ${CLIENT_DIR}client.c ${CLIENT_DIR}client_util.h
	gcc ${CFLAGS} -c ${CLIENT_DIR}client.c -o ${CLIENT_DIR}client.o

${CLIENT_DIR}client_util.o : ${CLIENT_DIR}client_util.c
	gcc ${CFLAGS} -c ${CLIENT_DIR}client_util.c -o ${CLIENT_DIR}client_util.o

clean: 
	\rm -rf ${SERVER_DIR}*.o ${CLIENT_DIR}*.o *~ c s