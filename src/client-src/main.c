#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>

#include "logging.h"
#include "client.h"
#include "client-commands.h"

int main(int argc, char *argv[]) {
	
	/* Requres 2 arguments */
	if ( argc < 3 ) {
		usage(argv[0]);
		return -1;
	}

	int port = atoi(argv[2]);

	if ( port < 1024 ) {
		error_msg("Port has to be above 1023");
		usage(argv[0]);
		return -1;
	}
	else if ( port > 65535 ) {
		error_msg("Port has to be below 65535");
		usage(argv[0]);	
		return -1;
	}

	int socket_fd;
	const char *protocol = "tcp";
	struct sockaddr_storage server = {0};

	printf("Creating socket...\n");
	if (client_create_socket(&socket_fd, protocol) < 0)
	{
		error_msg("Could not create socket");
		return -1;
	}

	pass_msg();

	printf("Attempting to connect to server...\n");
	if (client_connect_to_server(socket_fd, &server, port, argv[1]) < 0)
	{
		error_msg("Could not connect to server");
		close(socket_fd);
		return -1;
	}

	pass_msg();

	printf("Connected to %s:%s\n", argv[1], argv[2]);

	ssize_t send_bytes;
	ssize_t received_bytes;
	char request[BUFFER];
	size_t request_size = sizeof(request);
	char reply[BUFFER];
	size_t reply_size = sizeof(reply);
	command_t command;

	while (1)
	{
		memset(request, 0, request_size);

		client_received_bytes_from_server(socket_fd, &received_bytes, request, request_size);
		if (received_bytes < 0)
		{
			error_msg("Could not receive bytes from server");
			close(socket_fd);
			return -1;
		}

		memset(reply, 0, reply_size);
		client_request_from_server(reply, reply_size);

		command = command_return(reply);	

		if (command == UPLOAD)
			if (command_handle_upload(reply, reply_size) < 0)
				error_msg("Cant");

		client_send_bytes_to_server(socket_fd, &send_bytes, reply);
		if (send_bytes < 0)
		{
			error_msg("Could not receive bytes from server");
			close(socket_fd);
			return -1;
		}
	}

	close(socket_fd);
	return 0;
}
