#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>

#define BUFFER 1024

int client_create_socket(int *socket_fd, const char *protocol);
int client_connect_to_server(int socket_fd, struct sockaddr_storage *server, int port, char *address);
void client_request_from_server(char *reply, size_t reply_size);
void client_received_bytes_from_server(int socket_fd, ssize_t *received_bytes, char *request, size_t request_size);
void client_send_bytes_to_server(int socket_fd, ssize_t *send_bytes, char *reply);

#endif /* CLIENT_H */
