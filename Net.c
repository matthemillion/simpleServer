#include "Net.h"


const char *HEADER			=	"HTTP/1.1 200 OK\r\n"
				  				"Content-Type: text/html\r\n"
				  				"\r\n";


const char *FILE_NOT_FOUND	=	"<!DOCTYPE html>\r\n"
								"<html>\r\n"
								"<head>\r\n"
								"	<title>404 Error</title>\r\n"
								"</head>\r\n"
								"<body>\r\n"
								"	<h1>404 Error: File Not Found</h1>\r\n"
								"</body>\r\n"
								"</html>\r\n";


/**
 *	common functions for TCP-IP4 server
 *
********************************************************************/


// configure server address
int getIP4SocketBindAndListen(SA_in *server_p, uint16_t PORT, int back_log) {

	int fd_server;

	// clear memory for address
	memset(server_p, 0, sizeof(*server_p));

	// set up server address
	server_p->sin_family = AF_INET;				// internet
	server_p->sin_port = htons(PORT);			// host to network short
	server_p->sin_addr.s_addr = INADDR_ANY;		// 0

	// get socket from OS
	if((fd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror("server socket failure\n");
	    exit(1);
	} else
		printf("server socket: %i\n", fd_server);

	// bind server socket to server address
	if((bind(fd_server, (SA *) server_p, sizeof(SA))) < 0) {
		perror("socket binding failure\n");
		exit(1);
	} else
		printf("socket binding success\n");

	// listen for connection from clients
	if(listen(fd_server, back_log) < 0) {
		perror("failure to start listening\n");
		exit(1);
	} else
		printf("listening on port: %i\n", PORT);

	// return file descriptor for client
	return fd_server;
}


static int compareFileType(const char *file_name, const char *file_type) {

	int l_name = strlen(file_name), l_type = strlen(file_type);

	if(l_name <= l_type)
		return 0;

	for(int i = l_name - 1, j = l_type - 1; j >= 0; --i, --j)
		if(file_name[i] != file_type[j])
			return 0;

	return 1;
}


static int sendPage(int fd_client, const char *file_name) {

	struct stat st;
	int fd_storage;

	// account for unsanitary file names and files not found
	if(strstr(file_name, "..") != NULL || stat(file_name, &st) < 0) {
		write(fd_client, HEADER, strlen(HEADER));
		write(fd_client, FILE_NOT_FOUND, strlen(FILE_NOT_FOUND));
		return 0;
	}

	// open storage file
	if((fd_storage = open(file_name, O_RDONLY)) < 0) {
		perror("unable to open file\n");
		return 0;
	}

	// account for html file header
	if(compareFileType(file_name, "html"))
		write(fd_client, HEADER, strlen(HEADER));

	// send file to client
	sendfile(fd_client, fd_storage, NULL, st.st_size + 1);

	// close storage file
	close(fd_storage);

	return 1;
}


GetRequest * prepareGetRequest(int fd_client, const char *path, const char *buffer) {

	char c, *file_name;
	size_t left, right, l_path = strlen(path);
	GetRequest *getR;

	for(left = 0; buffer[left] != '\0'; ++left)
		if(buffer[left] == '/')
			break;

	// left index must be '/' in order to serve client
	if(buffer[left] != '/')
		return NULL;

	for(right = left; buffer[right] != '\0'; ++right) {
		c = buffer[right];

		if(!isalnum(c) && c != '.' && c != '-' && c != '/')
			break;
	}

	// right index must be greater than left index in order to serve client
	if(left == right)
		return NULL;

	size_t l_file_name = l_path + right - left;

	if((file_name = malloc((l_file_name + 1) * sizeof(char))) == NULL) {
		perror("memory allocation error\n");
		return NULL;
	}

	// path is put in front of file name
	strncpy(file_name, path, l_path);

	// file name is extracted from buffer
	for(size_t i = l_path; i < l_file_name && left < right; ++i, ++left)
		file_name[i] = buffer[left];

	// file name is null terminated
	file_name[l_file_name] = '\0';

	if((getR = malloc(sizeof(GetRequest))) == NULL) {
		free(file_name);
		return NULL;
	}

	getR->file_name = file_name, getR->fd_client = fd_client;

	return getR;
}


void * handleGetRequest(void *generic) {

	GetRequest *getR = (GetRequest *) generic;

	sendPage(getR->fd_client, getR->file_name);

	close(getR->fd_client), free(getR->file_name), free(getR);

	return NULL;
}

