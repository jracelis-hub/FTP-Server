#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "client.h"

int client_create_socket(int *socket_fd, const char *protocol)
{
	struct protoent *proto = getprotobyname(protocol);
	if (!proto) {
		return -2;
	}
#ifdef INET
	*socket_fd = socket(AF_INET, SOCK_STREAM, proto->p_proto);
	if (*socket_fd < 0) {
		return -1;
	}
	return 0;
#elif INET6
	*socket_fd = socket(AF_INET6, SOCK_STREAM, proto->p_proto);
	if (*socket_fd < 0) { 
		return -1;
	}
	return 0;
#endif
}

int client_connect_to_server(int socket_fd, struct sockaddr_storage *server, int port, char *address)
{
#ifdef INET
	struct sockaddr_in *server_ip4 = (struct sockaddr_in *)server;	
	server_ip4->sin_family = AF_INET;
	server_ip4->sin_port = htons(port);
	if (inet_pton(server_ip4->sin_family, address, 
	              &server_ip4->sin_addr) != 1) {
		return -1;
	}
	socklen_t addrlen = sizeof(*server_ip4);
	if (connect(socket_fd, (struct sockaddr *)server, addrlen) < 0) {
		return -1;
	}
	return 0;
#elif INET6
	struct sockaddr_in6 *server_ip6 = (struct sockaddr_in6 *)server;
	server_ip6->sin6_family = AF_INET6;
	server_ip6->sin6_port = htons(port);
	if (inet_pton(server_ip6->sin6_family, address,
	              &server_ip6->sin6_addr) != 1) {
		return -1;
	}
	socklen_t addrlen = sizeof(*server_ip6);
	if (connect(socket_fd, (struct sockaddr *)server, addrlen) < 0) {
		return -1;
	}
	return 0;
#endif
}

void client_received_bytes_from_server(int socket_fd, ssize_t *received_bytes, char *request, size_t request_size)
{
	*received_bytes = recv(socket_fd, request, request_size, 0);
	if (*received_bytes == 0) {
		return;
	}
	else if (*received_bytes < 0) {
		return;
	}
	
	request[*received_bytes] = '\0';

	printf("Received %zu bytes\n", *received_bytes);
	printf("%s", request);
}

void client_request_from_server(char *reply, size_t reply_size)
{
	size_t input_len;
	printf("$ ");
	fgets(reply, reply_size, stdin);

	input_len = strlen(reply);
	reply[input_len] = '\0';
}

void client_send_bytes_to_server(int socket_fd, ssize_t *send_bytes, char *reply)
{
	*send_bytes = send(socket_fd, reply, strlen(reply), 0);
	if (*send_bytes == 0) {
		return;
	}
	else if (*send_bytes < 0) {
		return;
	}

	printf("Send %zu bytes\n", *send_bytes);
}
