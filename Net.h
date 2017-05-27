#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 *	common functions for TCP-IP4 server
 *
********************************************************************/

typedef struct sockaddr_in SA_in;

typedef struct sockaddr	SA;

int getIP4SocketBindAndListen(SA_in *, uint16_t, int);

typedef struct {
	int fd_client;
	char *file_name;
} GetRequest;

GetRequest * prepareGetRequest(int, const char *, const char *);

void * handleGetRequest(void *);

#endif // NET_H_INCLUDED

