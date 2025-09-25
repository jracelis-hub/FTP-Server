/* Standard Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Socket Headers */
#include <netdb.h>

/* User Define Headers */
#include "server.h"
#include "commands.h"
#include "logging.h"
#include "thread.h"

void thread_set_thread_handler(thread_handler_t *thread_handle, server_thread_t *thread_args)
{
	/* Set buffer sizes */
	thread_handle->request_size = MINUMUM_BUFFER_SIZE;
	thread_handle->reply_size = MINUMUM_BUFFER_SIZE;
	thread_handle->file_size = FILE_PATH_SIZE;
	thread_handle->directory_size = DIRECTORY_SIZE;

	/* Make sure buffers are zero'd out */
	memset(thread_handle->request, 0, thread_handle->request_size);
	memset(thread_handle->reply, 0, thread_handle->reply_size);
	memset(thread_handle->file, 0, thread_handle->file_size);
	memset(thread_handle->directory, 0, thread_handle->directory_size);

	/* Allocate heap values passed into thread */
	thread_handle->thread_fd = thread_args->client_fd;
	thread_handle->client_address = thread_args->client_address;
	strcpy(thread_handle->directory, thread_args->directory);

	thread_handle->command = INVALID;
	thread_handle->receive_bytes = 0;
	thread_handle->send_bytes = 0;
}

void thread_set_buffers_zero(thread_handler_t *thread_handle)
{
	memset(thread_handle->request, 0, thread_handle->request_size);
	memset(thread_handle->reply, 0, thread_handle->reply_size);
}

void thread_send_welcome_message(int client_fd, ssize_t *send_bytes)
{
	const char *welcome_message = 
	"--------------------------------\n"
	"|  Welcome to RPi FTP Server   |\n"
	"|------------------------------|\n"
	"|  Use the following commands: |\n"
	"|-------------------------------\n"
	"|  Download; file  |\n"
	"|  Upload; file    |\n"
	"|  List;           |\n"
	"|  Read; file      |\n"
	"--------------------\n";

	/* Send welcome message + '\0' byte */
	*send_bytes = send(client_fd, welcome_message, 
	                   strlen(welcome_message) + 1, 0);
	if (*send_bytes == 0) return;
	else if (*send_bytes < 0) return;
}

void *thread_handle_client(void *args)
{
	server_thread_t *thread_client = (server_thread_t *)args;

	/* thread_handle will hold all data needed to communicate with the client */
	thread_handler_t thread_handle;
	thread_set_thread_handler(&thread_handle, thread_client);

	/* Heap values are now stored on the stack so now free the memory */
	free(thread_client);

	if (thread_print_accept_connection_message(&thread_handle.client_address) < 0)
	{
		error_msg("Could not get ip and port of client");
		if (thread_shutdown_client(thread_handle.thread_fd) < 0) {
			error_msg("Unable to shutdown the client properly");
		}
		if (thread_print_disconnect_message(&thread_handle.client_address) < 0) {
			error_msg("Could not print disconect message");
		}
		return NULL;
	}

	thread_send_welcome_message(thread_handle.thread_fd, &thread_handle.send_bytes);
	if (thread_handle.send_bytes < 0)
	{
		error_msg("Could not send bytes");
		if (thread_shutdown_client(thread_handle.thread_fd) < 0) {
			error_msg("Unable to shutdown the client properly");
		}
		if (thread_print_disconnect_message(&thread_handle.client_address) < 0) {
			error_msg("Could not print disconect message");
		}
		return NULL;
	}

	while (1) {

		thread_set_buffers_zero(&thread_handle);

		thread_receive_bytes_from_client(&thread_handle);
		if (thread_handle.receive_bytes < 0) {
			error_msg("Could not receive bytes");
			break;
		}
		
		command_receive_from_client(&thread_handle);

		if (command_handler(&thread_handle) < 0) {
			error_msg("Could not perform command properly");
			break;
		}

		thread_send_bytes_to_client(&thread_handle);
		if (thread_handle.send_bytes < 0) {
			error_msg("Could not send bytes");
			break;
		}

		if (thread_handle.command == INVALID) {
			break;
		}
	}

	if (thread_shutdown_client(thread_handle.thread_fd) < 0) {
		error_msg("Unable to shutdown the client properly");
	}
	if (thread_print_disconnect_message(&thread_handle.client_address) < 0) {
		error_msg("Could not print disconect message");
	}
	return NULL;
}

void thread_receive_bytes_from_client(thread_handler_t *thread_handle)
{
	thread_handle->receive_bytes = recv(thread_handle->thread_fd, thread_handle->request, 
	                                     thread_handle->request_size, 0);
	if (thread_handle->receive_bytes == 0) return;
	else if (thread_handle->receive_bytes < 0) return;

	thread_handle->request[thread_handle->receive_bytes] = '\0';

	printf("Received %zu bytes\n", thread_handle->receive_bytes);
	printf("Request: %s", thread_handle->request);
}

void thread_send_bytes_to_client(thread_handler_t *thread_handle)
{
	size_t reply_len = strlen(thread_handle->reply);
	thread_handle->send_bytes = send(thread_handle->thread_fd, thread_handle->reply, reply_len + 1, 0);
	if (thread_handle->send_bytes == 0) return;
	else if (thread_handle->send_bytes < 0) return;

	printf("Send %zu bytes\n",thread_handle->send_bytes);
}

int thread_shutdown_client(int thread_fd)
{
	if (shutdown(thread_fd, SHUT_RDWR) != 0) {
		return ERROR_SHUTDOWN;
	}
	close(thread_fd);	

	return SUCCESS;
}

int thread_print_disconnect_message(struct sockaddr_storage *client_address)
{
	socklen_t addrlen = sizeof(*client_address);
	char client_ip[NI_MAXHOST];
	size_t len_ip = sizeof(client_ip);
	char client_port[NI_MAXSERV];
	size_t len_port = sizeof(client_port);

	if (getnameinfo((struct sockaddr *)client_address, addrlen,
	            client_ip, len_ip,
				client_port, len_port,
				NI_NUMERICHOST | NI_NUMERICSERV) != 0)
		return ERROR_NAMEINFO;

	printf("Disconnect from %s:%s\n", client_ip, client_port);

	return SUCCESS;
}

int thread_print_accept_connection_message(struct sockaddr_storage *client_address)
{
	socklen_t addrlen = sizeof(*client_address);
	char client_ip[NI_MAXHOST];
	size_t len_ip = sizeof(client_ip);
	char client_port[NI_MAXSERV];
	size_t len_port = sizeof(client_port);

	if (getnameinfo((struct sockaddr *)client_address, addrlen,
	            client_ip, len_ip,
				client_port, len_port,
				NI_NUMERICHOST | NI_NUMERICSERV) != 0)
		return ERROR_NAMEINFO;

	printf("Accepted connection from %s:%s\n", client_ip, client_port);

	return SUCCESS;
}
