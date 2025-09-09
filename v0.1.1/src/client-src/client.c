#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "net_utility.h"
#include "client_utility.h"

int main(int argc, char *argv[]) {
	
	/* Requres 2 arguments */
	if ( argc < 3 ) {
		usage(argv);
		return -1;
	}

	int port = atoi(argv[2]);

	if ( port < 1024 ) {
		error_msg("Port has to be above 1023");
		usage(argv);
		return -1;
	}
	else if ( port > 65535 ) {
		error_msg("Port has to be below 65535");
		usage(argv);	
		return -1;
	}

	char reply[BUFFER];
	char request[BUFFER];

	int status;

	/* To get protocol */
	const char *proto = "tcp";	
	struct protoent *proto_name = getprotobyname(proto);

	perform_task("Creating socket...",NULL);
	int sock_fd = socket(AF_INET,SOCK_STREAM,proto_name->p_proto);
	if ( sock_fd < 0 ) {
		error_msg("Could not create socket");
		return -1;
	}

	pass_msg(NULL,NULL);

	struct sockaddr_in server;
	memset(&server,0,sizeof(server));

	/* Setting up server structure information */
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	status = inet_pton(AF_INET,argv[1],&server.sin_addr);
	if ( status < 0 ) {
		error_msg("Could not convert address to network order");
		close(sock_fd);
		return -1;
	}

	perform_task("Attempting to connect to server...",NULL);
	status = connect(sock_fd,(struct sockaddr *)&server,sizeof(server));
	if ( status != 0 ) {
		error_msg("Could not connect to server");
		close(sock_fd);
		return -1;
	}
	
	pass_msg(NULL,NULL);

	printf("Connected to %s:%s\n",argv[1],argv[2]);

	while (1) {

		memset(reply,0,sizeof(reply));
		memset(request,0,sizeof(request));

		status = get_request(sock_fd,request,sizeof(request));
		if ( status < 0 ) {
			error_msg("Could not receive response from server");
			close(sock_fd);
			return -1;
		}

		status = check_for_exit(request);
		if ( status == 0 ) {
			printf("Ending connection\n");
			close(sock_fd);
			return 0;
		}

		status = send_request(sock_fd,reply,sizeof(reply));
		if ( status < 0 ) {
			error_msg("Could not send response to server");
			close(sock_fd);
			return -1;
		}
	}

	close(sock_fd);
	
	return 0;
}
