#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "logging.h"
#include "commands-client.h"
#include "parsing.h"

void command_cache_request(client_handler_t *client_handle) 
{
	if (!strncmp(client_handle->reply, "Download;", strlen("Download;")))
	{
		if (parse_request_get_file_path(client_handle->reply, 
		                               client_handle->file,
									   client_handle->file_size) < 0) 
		{
			error_msg("Error overflow");
			return;
		}
		client_handle->command = DOWNLOAD;
	}
	else if (!strncmp(client_handle->reply, "Upload;", strlen("Upload;")))
	{
		client_handle->command = UPLOAD;
	}
	else if (!strncmp(client_handle->reply, "List;", strlen("List;")))
	{
		client_handle->command = LIST;
	}
	else if (!strncmp(client_handle->reply, "Read;", strlen("Read;")))
	{
		client_handle->command = READ;
	}
	else
	{
		client_handle->command = INVALID;
	}
}

void command_handler(client_handler_t *client_handle)
{
	switch (client_handle->command)
	{
		case DOWNLOAD:
			break;
		case UPLOAD:
			break;
		case LIST:
			break;
		case READ:
			break;
		case EMPTY:
			command_handle_empty(client_handle);
			break;
		default:
	}
}

void command_handle_empty(client_handler_t *client_handle) 
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

