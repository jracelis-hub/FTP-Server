#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "commands-client.h"
#include "parsing.h"

void command_cache_request(client_handler_t *client_handle) 
{
	if (!strncmp(client_handle->reply, "Download;", strlen("Download;")))
	{
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


