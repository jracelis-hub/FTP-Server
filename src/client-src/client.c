#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "client.h"
#include "logging.h"

/* To create TCP IPv4 socket */
int client_create_stream_socket_ip4(int *socket_fd)
{
	const char *protocol = "tcp";
	struct protoent *proto = getprotobyname(protocol);
	if (!proto) 
	{
		return -2;
	}
	*socket_fd = socket(AF_INET, SOCK_STREAM, proto->p_proto);
	if (*socket_fd < 0) 
	{
		return -1;
	}
	return 0;
}

/* To connect to a IPv4 address */
int client_connect_to_server_ip4(int socket_fd, struct sockaddr_storage *server, int port, char *address)
{
	struct sockaddr_in *server_ip4 = (struct sockaddr_in *)server;	
	server_ip4->sin_family = AF_INET;
	server_ip4->sin_port = htons(port);
	if (inet_pton(server_ip4->sin_family, address, 
	              &server_ip4->sin_addr) != 1) 
	{
		return -1;
	}
	socklen_t addrlen = sizeof(*server_ip4);
	if (connect(socket_fd, (struct sockaddr *)server, addrlen) < 0) 
	{
		return -1;
	}
	return 0;
}

/* To create TCP IPv6 socket */
int client_create_stream_socket_ip6(int *socket_fd)
{
	const char *protocol = "tcp";
	struct protoent *proto = getprotobyname(protocol);
	if (!proto) 
	{
		return -2;
	}
	*socket_fd = socket(AF_INET6, SOCK_STREAM, proto->p_proto);
	if (*socket_fd < 0) 
	{
		return -1;
	}
	return 0;
}

/* To connect to a IPv6 address */
int client_connect_to_server_ip6(int socket_fd, struct sockaddr_storage *server, int port, char *address)
{
	struct sockaddr_in6 *server_ip6 = (struct sockaddr_in6 *)server;
	server_ip6->sin6_family = AF_INET6;
	server_ip6->sin6_port = htons(port);
	if (inet_pton(server_ip6->sin6_family, address,
	              &server_ip6->sin6_addr) != 1) 
	{
		return -1;
	}
	socklen_t addrlen = sizeof(*server_ip6);
	if (connect(socket_fd, (struct sockaddr *)server, addrlen) < 0) 
	{
		return -1;
	}
	return 0;
}

/* Prints out server information */
int client_print_connection_message(struct sockaddr_storage *server_address) 
{
	socklen_t server_len = sizeof(*server_address);
	char server_ip[NI_MAXHOST];
	size_t len_ip = sizeof(server_ip);
	char server_port[NI_MAXSERV];
	size_t len_port = sizeof(server_port);

	if (getnameinfo((struct sockaddr *)server_address, server_len,
	                server_ip, len_ip, 
		            server_port, len_port,
		            NI_NUMERICHOST | NI_NUMERICSERV) != 0) 
	{
		error_msg("Could not get server address information");
		return -1;
	}

	printf("Connected to %s:%s\n", server_ip, server_port);
	return 0;
}

/* Create a client handler to handle all communication for server <-> client communication */
void client_create_client_handler(client_handler_t *client_handle, int socket_fd)
{
	client_handle->request_size = MINUMUM_BUFFER_SIZE;
	client_handle->reply_size = MINUMUM_BUFFER_SIZE;
	client_handle->file_size = FILE_SIZE;

	memset(client_handle->request, 0, client_handle->request_size);
	memset(client_handle->reply, 0, client_handle->reply_size);
	memset(client_handle->file, 0, client_handle->file_size);

	client_handle->socket_fd = socket_fd;
	client_handle->receive_bytes = 0;
	client_handle->send_bytes = 0;
	client_handle->command = EMPTY;
}

void client_set_buffers_zero(client_handler_t *client_handle)
{
	memset(client_handle->request, 0, client_handle->request_size);
	memset(client_handle->reply, 0, client_handle->reply_size);
}

void client_received_bytes_from_server(client_handler_t *client_handle)
{
	if (client_handle->command == EMPTY)
	{
		client_handle->received_bytes = recv(client_handle->socket_fd, client_handle->request, 
											 client_handle->request_size, 0);
		if (client_handle->received_bytes == 0) 
		{
			return;
		}
		else if (client_handle->received_bytes < 0) 
		{
			return;
		}
		
		client_handle->request[client_handle->received_bytes] = '\0';

		printf("Received %zu bytes\n", client_handle->received_bytes);
		printf("%s", client_handle->request);
	}

	client_handle->received_bytes = recv(client_handle->socket_fd, client_handle->request, 
										 client_handle->request_size, 0);
	if (client_handle->received_bytes == 0) 
	{
		return;
	}
	else if (client_handle->received_bytes < 0) 
	{
		return;
	}
	
	client_handle->request[client_handle->received_bytes] = '\0';

	printf("Received %zu bytes\n", client_handle->received_bytes);
	printf("%s", client_handle->request);
}

void client_send_request_to_server(client_handler_t *client_handle)
{
	size_t input_len;
	printf("$ ");
	fgets(client_handle->reply, client_handle->reply_size, stdin);

	input_len = strlen(client_handle->reply);
	reply[input_len] = '\0';
}

void client_send_bytes_to_server(client_handler_t *client_handle)
{
	client_handle->send_bytes = send(client_handle->socket_fd, client_handle->reply, 
	                   strlen(client_handle->reply), 0);
	if (client_handle->send_bytes == 0) 
	{
		return;
	}
	else if (client_handle->send_bytes < 0) 
	{
		return;
	}

	printf("Send %zu bytes\n", client_handle->send_bytes);
}
