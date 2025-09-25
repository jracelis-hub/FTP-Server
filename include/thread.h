#ifndef THREAD_H
#define THREAD_H

/* 8 kb of buffer size */
#define MINUMUM_BUFFER_SIZE 8192
#define FILE_PATH_SIZE 128

#include <sys/types.h> /* ssize_t */
#include <sys/socket.h> /* struct sockaddr_storage */
#include <stddef.h> /* size_t */

#include "command_type.h" /* command_t */

/* Declartion in server.h */
typedef struct server_thread_t server_thread_t;

/* thread_handler_t handles all the thread/command operations */
typedef struct thread_handler_t {
	int thread_fd;
	command_t command;
	char request[MINUMUM_BUFFER_SIZE];
	char reply[MINUMUM_BUFFER_SIZE];
	char directory[MINUMUM_BUFFER_SIZE];
	char file[FILE_PATH_SIZE];
	size_t request_size;
	size_t reply_size;
	size_t file_size;
	size_t directory_size;
	ssize_t receive_bytes;
	ssize_t send_bytes;
	struct sockaddr_storage client_address;
} thread_handler_t;

/* thread_handle_client is the function that gets passed into 
 * server_create_thread to perform thread->client communications */
void *thread_handle_client(void *args);

/* Set thread_handler_t items and transfer heap memory to corresponding values on the stack */
void thread_set_thread_handler(thread_handler_t *thread_handle, server_thread_t *thread_args);

/* thread utilities that prints accept and disconnection message */
int thread_print_accept_connection_message(struct sockaddr_storage *client_address);
int thread_print_disconnect_message(struct sockaddr_storage *client_address);
void thread_send_welcome_message(int client_fd, ssize_t *send_bytes);

/* Send and receive operation thread <-> client */
void thread_receive_bytes_from_client(thread_handler_t *thread_handle);
void thread_send_bytes_to_client(thread_handler_t *thread_handle);

/* Shutdown full-duplex communication from thread <-> client and closes socket fd */
int thread_shutdown_client(int thread_fd);

#endif  /* THREAD_H */
