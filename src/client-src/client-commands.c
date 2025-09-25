#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "client-commands.h"
#include "command_utility.h"

#define BUFFER 1024

command_t command_return(char *request) 
{
	command_t command;

	if (!strncmp(request, "Download;", strlen("Download;")))
		command = DOWNLOAD;
	else if (!strncmp(request, "Upload;", strlen("Upload;")))
		command = UPLOAD;
	else if (!strncmp(request, "List;", strlen("List;")))
		command = LIST;
	else if (!strncmp(request, "Read;", strlen("Read;")))
		command = READ;
	else command = INVALID;

	return command;
}

void command_get_file(char *request, char *file, int size)
{
	int i = 0;
	char *p;	
	for (p = request; *p != '\0'; p++)
	{
		if (isasemicolon(*p) || isaspace(*p))
		{
			request = p;
			request++;
		}
		if (!strncmp(p, "..", 2))
			request = p;
	}

	while (*request != '\0' && *request != '\n' && i < size - 1) 
		file[i++] = *request++;
	file[i] = '\0';
}

int command_handle_upload(char *reply, size_t reply_size)
{
	char file[128];
	memset(file,0,sizeof(file));

	char temp[BUFFER];

	command_get_file(reply, file, sizeof(file));

	int fd = open(file, O_RDONLY);
	if (fd < 0) return -2;

	ssize_t bytes = read(fd, temp, sizeof(temp) - 1);
	if (bytes < 0) return -1;
	if (bytes == 0) return -2;

	size_t len = strlen(temp);

	strncat(reply, temp, len - 1);

	close(fd);	

	return 0;
}
