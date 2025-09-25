/* Standard Headers */
#include <stdlib.h>
#include <string.h>

/* Socket Headers */
#include <arpa/inet.h>
#include <netdb.h>

/* Pthread Header */
#include <pthread.h>

/* User Define Headers */
#include "server.h"
#include "logging.h"
#include "thread.h"

/* To create a ipv4 server */
int server_set_ip4(struct sockaddr_in *server_ip4, int port, char *ip4_address)
{
	server_ip4->sin_family = AF_INET;
	server_ip4->sin_port = htons(port);
	if (inet_pton(server_ip4->sin_family, ip4_address, 
	              &server_ip4->sin_addr) != 1)
		return ERROR_SERVER;
	return SUCCESS;
}

int server_create_stream_socket_ip4(int *listen_fd)
{
	const char *proto_name = "tcp";
	struct protoent *proto = getprotobyname(proto_name);
	if (!proto) {
		return ERROR_SERVER;
	}

	*listen_fd = socket(AF_INET, SOCK_STREAM, proto->p_proto);
	if (*listen_fd < 0) {
		return ERROR_SERVER;
	}

	return SUCCESS;
}

int server_bind_socket_ip4(int listen_fd, struct sockaddr_in *server_ip4)
{
	socklen_t addrlen = sizeof(*server_ip4);

	if (bind(listen_fd, (struct sockaddr *)server_ip4, addrlen) != 0) {
		return ERROR_SERVER;
	}

	return SUCCESS;
}

/* To create a ipv6 server */
int server_set_ip6(struct sockaddr_in6 *server_ip6, int port, char *ip6_address)
{
	server_ip6->sin6_family = AF_INET6;
	server_ip6->sin6_port = htons(port);
	if (inet_pton(server_ip6->sin6_family, ip6_address, 
	              &server_ip6->sin6_addr) != 1) {
		return ERROR_SERVER;
	}

	return SUCCESS;
}

int server_create_stream_socket_ip6(int *listen_fd)
{
	const char *proto_name = "tcp";
	struct protoent *proto = getprotobyname(proto_name);
	if (!proto) {
		return ERROR_SERVER;
	}

	*listen_fd = socket(AF_INET6, SOCK_STREAM, proto->p_proto);
	if (*listen_fd < 0) {
		return ERROR_SERVER;
	}

	return SUCCESS;
}

int server_bind_socket_ip6(int listen_fd, struct sockaddr_in6 *server_ip6)
{
	socklen_t addrlen = sizeof(*server_ip6);

	if (bind(listen_fd, (struct sockaddr *)server_ip6, addrlen) != 0) {
		return ERROR_SERVER;
	}

	return SUCCESS;
}

/* Use socket created to begin listening */
int server_listen_on_socket(int listen_fd)
{
	if (listen(listen_fd, MAX_WAITING_CONNECTIONS) != 0) {
		return ERROR_SERVER;
	}

	return SUCCESS;
}

int server_accept_client(int listen_fd, int *client_fd, struct sockaddr_storage *client)
{
		socklen_t addrlen = sizeof(*client);

		*client_fd = accept(listen_fd, (struct sockaddr *)client, &addrlen);
		if (*client_fd < 0) {
			return ERROR_SERVER;
		}
	return SUCCESS;
}

int server_set_directory(char *directory, size_t directory_size, char *directory_arg)
{
	/* Checks if the directory ends with a / if not it will append it to the end */
	size_t dir_arg_len = strlen(directory_arg);
	if (directory_arg[dir_arg_len - 1] != '/' && dir_arg_len < directory_size - 1) {
		strcpy(directory, directory_arg);	
		strcat(directory, "/");
		return SUCCESS;
	} else if (directory_arg[dir_arg_len - 1] == '/') {
		return SUCCESS;
	}
	return ERROR_DIR;
}

int server_create_thread(int client_fd, char *directory, struct sockaddr_storage *client_address, void *(*thread_handle_client)(void *))
{
	/* Allocates space for server_pthreads_t on the heap to pass the 
	 * the arguments utilized in thread_handle_client             */
	server_thread_t *server_thread = calloc(1, sizeof(*server_thread));
	if (server_thread == NULL) {
		error_msg("Could not allocate space");
		return ERROR_ALLOC;
	}
	
	/* server_pthread->thread_client_fd holds the value socket fd 
	 * server_pthread->directory holds argv[3] (mounted directory)
	 * server_pthread->client_address holds client_address information
	 */
	 server_thread->client_fd = client_fd;
	 server_thread->directory = directory;
	 server_thread->client_address = *client_address;

	/* Create pthread attr to set for pthread_create */
	pthread_attr_t attr;
	if (pthread_attr_init(&attr) != 0) {
		error_msg("Could not create pthread attribute");
		free(server_thread);
		return ERROR_THREADS;
	}
	
	/* pthread_attr_setdetachstate is set to detach 
	 * the detach state allows each thread to handle its own 
	 * communicate with the client and * free its resources when done                   
	 */
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
		error_msg("Could not set pthread attr to detached state");
		pthread_attr_destroy(&attr);
		free(server_thread);
		return ERROR_THREADS;
	}

	/* Create a pthread */
	pthread_t thread_client;
	if (pthread_create(&thread_client, &attr, thread_handle_client, (void *)server_thread) != 0) {
		error_msg("Could not create thread");
		pthread_attr_destroy(&attr);
		free(server_thread);
		return ERROR_THREADS;
	}

	pthread_attr_destroy(&attr);

	return SUCCESS;
}
