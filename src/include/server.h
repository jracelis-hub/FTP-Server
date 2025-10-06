#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h> /* struct sockaddr_storage */
#include <netinet/in.h> /* struct sockaddr_in & sockaddr_in6 */
#include <stddef.h> /* size_t */

#define MAX_WAITING_CONNECTIONS 5
#define DIRECTORY_SIZE 256

/* server_thread_t is used to be passed into void *args 
 * of thread_handle_client found in thread.h         */
typedef struct server_thread_t {
	int client_fd;
	char *directory;
	struct sockaddr_storage client_address;
} server_thread_t;

/* Transfers argv[3] mounted directory to the stack directory */
int server_set_directory(char *directory, size_t directory_size, char *dir_arg);

/* Sets socket option */
int server_set_socket_options(int *listen_fd);

/* IPv4 Setup */
int server_create_stream_socket_ip4(int *listen_fd);
int server_set_ip4(struct sockaddr_in *server_ip4, int port, char *ip4_address);
int server_bind_socket_ip4(int listen_fd, struct sockaddr_in *server_ip4);

/* IPv6 Setup */
int server_create_stream_socket_ip6(int *listen_fd);
int server_set_ip6(struct sockaddr_in6 *server_ip6, int port, char *ip6_address);
int server_bind_socket_ip6(int listen_fd, struct sockaddr_in6 *server_ip6);

/* Passive Socket and Accept Client Socket */
int server_listen_on_socket(int listen_fd);
int server_accept_client(int listen_fd, int *client_fd, struct sockaddr_storage *client);

/* Create Thread */
int server_create_thread(int client_fd, char *directory, struct sockaddr_storage *client_address, void *(*server_handle_client)(void *));

#endif /* SERVER_H */
