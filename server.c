#include "Net.h"
#include "ThreadPool.h"


int main() {

	SA_in server, client;
	uint16_t PORT = 9999;
	int back_log = 512, fd_client;

	int fd_server = getIP4SocketBindAndListen(&server, PORT, back_log);

	socklen_t socket_length;

	const size_t SIZE = 1024;
	char request_buffer[SIZE + 1], *path = "public";

	GetRequest *getR;

	ThreadPool TP;
	
	// use 2 worker threads with a queue capacity of 1024
	if(!ThreadPoolStartUp(&TP, 2, 1024)) {
		perror("unable to start thread pool\n");
		exit(1);
	}

	// handle clients on infinite loop
	while(1) {

		socket_length = sizeof(SA_in);
		memset(&client, 0, sizeof(client));

		// accept client
		if((fd_client = accept(fd_server, (SA *) &client, &socket_length)) < 0) {
			perror("client not accepted\n");
			continue;
		}

		// recieve GET request from client
		memset(request_buffer, '\0', (SIZE + 1) * sizeof(char));
		read(fd_client, request_buffer, SIZE);
		printf("%s\n", request_buffer);

		// prepare GET request
		if((getR = prepareGetRequest(fd_client, path, request_buffer)) == NULL)
			continue;

		// handle GET request
		ThreadPoolScheduleTask(&TP, handleGetRequest, getR);
	}
}

