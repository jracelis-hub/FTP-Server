#include "client_utility.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int get_request(int sock_fd,char *request,size_t request_s) {

	ssize_t bytes = recv(sock_fd,request,request_s,0);
	if ( bytes < 0 ) {
		return -1;
	} else if ( bytes == 0 ) {
		return 2;
	}

	printf("Received: %zu bytes\n",bytes);
	printf("%s",request);

	return 0;
}

int send_request(int sock_fd,char *reply,size_t reply_s) {
	
	printf("$ ");
	fgets(reply,reply_s,stdin);
	ssize_t bytes = send(sock_fd,reply,strlen(reply),0);
	if ( bytes < 0 ) {
		return -1;
	}

	return 0;
}
