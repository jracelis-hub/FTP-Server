#ifndef CLIENT_H
#define CLIENT_H

/* Includes the typedef enum command_t */
#include "command_type.h"
#include <sys/socket.h>

/* 4 KiB of */
#define MINUMUM_BUFFER_SIZE 4096
#define FILE_SIZE 256

/* Holds all of the client items to perform reads and writes 
   to and from the server and files                       */
typedef struct client_handler_t {
	int socket_fd;
	command_t command;
	char request[MINUMUM_BUFFER_SIZE];
	char reply[MINUMUM_BUFFER_SIZE];
	char file[FILE_SIZE];
	size_t request_size;
	size_t reply_size;
	size_t file_size;
	ssize_t receive_bytes;
	ssize_t send_bytes;
} client_handler_t;

/* Creates tcp ipv4 socket type */
int client_create_stream_socket_ip4(int *socket_fd);
int client_connect_to_server_ip4(int socket_fd, struct sockaddr_storage *server, int port, char *address);

/* Creates tcp ipv6 socket type */
int client_create_stream_socket_ip6(int *socket_fd);
int client_connect_to_server_ip6(int socket_fd, struct sockaddr_storage *server, int port, char *address);

/* Client utility to print of server information */
int client_print_connection_message(struct sockaddr_storage *server);

/* Create client handler to be used when communicating to server */
void client_create_client_handler(client_handler_t *client_handle, int socket_fd);

void client_set_buffers_zero(client_handler_t *client_handle);

void client_send_request_to_server(client_handler_t *client_handle);
void client_received_bytes_from_server(client_handler_t *client_handle);
void client_send_bytes_to_server(client_handler_t *client_handle);

#endif /* CLIENT_H */
