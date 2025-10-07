#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "logging.h"
#include "commands-client.h"
#include "client.h"
#include "parsing.h"

void command_handler(client_handler_t *client_handle)
{
	switch (client_handle->command)
	{
		case DOWNLOAD:
			printf("Hello\n");
			break;
		case UPLOAD:
			printf("Hello\n");
			break;
		case LIST:
			command_handle_list(client_handle);
			break;
		case READ:
			command_handle_read(client_handle);
			break;
		case INVALID:
			printf("Hello\n");
			break;
		case EMPTY:
			command_handle_empty(client_handle);
			break;
	}
}

void command_handle_empty(client_handler_t *client_handle) 
{
	client_handle->receive_bytes = recv(client_handle->socket_fd, client_handle->request, 
										 client_handle->request_size, 0);
	if (client_handle->receive_bytes == 0) 
	{
		return;
	}
	else if (client_handle->receive_bytes < 0) 
	{
		return;
	}
	
	client_handle->request[client_handle->receive_bytes] = '\0';

	printf("Received %zu bytes\n", client_handle->receive_bytes);
	printf("%s", client_handle->request);
}

void command_handle_read(client_handler_t *client_handle) 
{
	client_handle->receive_bytes = recv(client_handle->socket_fd, client_handle->request, 
										 client_handle->request_size, 0);
	if (client_handle->receive_bytes == 0) 
	{
		return;
	}
	else if (client_handle->receive_bytes < 0) 
	{
		return;
	}
	
	client_handle->request[client_handle->receive_bytes] = '\0';

	printf("Received %zu bytes\n", client_handle->receive_bytes);
	printf("%s", client_handle->request);
}

int command_handle_upload(client_handler_t *client_handle)
{
	int fd;
	ssize_t read_bytes;
	const int buffer = 4096;
	char temp[buffer];
	memset(temp, 0, buffer);

	parse_request_get_file_path(client_handle->reply, client_handle->file, client_handle->file_size);

	fd = open(client_handle->file, O_RDONLY);
	if (fd < 0) 
	{
		error_msg("Could not open file");
		return ERROR_FILE;
	}
	
	read_bytes = read(fd, temp, buffer);
	if (read_bytes < 0)
	{
		error_msg("Could not read bytes");
		return ERROR_BYTES;
	}

	if (strlen(temp) + strlen(client_handle->reply) + 1
	    > client_handle->reply_size)
	{
		error_msg("Overflow");
		return ERROR_OVERFLOW;
	}
	strcat(client_handle->reply, temp);

	close(fd);

	return SUCCESS;
}

void command_handle_list(client_handler_t *client_handle) 
{
	client_handle->receive_bytes = recv(client_handle->socket_fd, client_handle->request, 
										 client_handle->request_size, 0);
	if (client_handle->receive_bytes == 0) 
	{
		return;
	}
	else if (client_handle->receive_bytes < 0) 
	{
		return;
	}
	
	client_handle->request[client_handle->receive_bytes] = '\0';

	printf("Received %zu bytes\n", client_handle->receive_bytes);
	printf("%s", client_handle->request);
}
