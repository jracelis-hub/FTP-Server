#include "commands.h" 
#include "parsing.h"
#include "logging.h"
#include "thread.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

void command_receive_from_client(thread_handler_t *thread_handle)
{
	if (!strncmp(thread_handle->request, "Download;", strlen("Download;"))) {
		thread_handle->command = DOWNLOAD;
	}
	else if (!strncmp(thread_handle->request, "Upload;", strlen("Upload;"))) {
		thread_handle->command = UPLOAD;
	}
	else if (!strncmp(thread_handle->request, "List;", strlen("List;"))) {
		thread_handle->command = LIST;
	}
	else if (!strncmp(thread_handle->request, "Read;", strlen("Read;"))) {
		thread_handle->command = READ;
	}
	else {
		thread_handle->command = INVALID;
	}
}

int command_handler(thread_handler_t *thread_handle)
{
	switch (thread_handle->command) {
		case DOWNLOAD:
			break;
		case UPLOAD:
			break;
		case LIST:
			if (command_handle_list(thread_handle) < 0) {
				return ERROR_COMMAND;
			}
			break;
		case READ:
			break;
		case INVALID:
			command_handle_invalid(thread_handle);
			break;
	}
	return SUCCESS;
}

int command_handle_upload(char *request, char *reply, size_t reply_size, char *directory)
{
	char file_path[256] = {0};
	size_t file_path_len = sizeof(file_path);

	char file[64] = {0};
	size_t file_len = sizeof(file);

	if (command_get_file(request, file, file_len) < 0)
	{
		error_msg("Command_upload = overflow");
		return ERROR_OVERFLOW;
	}

	printf("File = %s",file);

	if (!isdirectoryformat(file_path))
		snprintf(file_path, file_path_len, "%s/%s", directory, file);
	else
		snprintf(file_path, file_path_len, "%s%s", directory, file);

	printf("File path = %s",file_path);

	int fd = open(file_path, O_WRONLY, S_IRWXU);
	if (fd == -1) 
	{
		error_msg("Could not open file");
		return ERROR_FILE;
	}

	char *payload = command_strip_header(request);

	ssize_t write_bytes = write(fd, payload, strlen(payload));
	if (write_bytes == -1) return ERROR_BYTES;
	else if (write_bytes > 0) reply[write_bytes] = '\0';

	close(fd);

	return SUCCESS;
}

int command_handle_read(thread_handler_t *thread_handle)
{
	/* Parse request to get the file:
	 * Read; file.txt and strip off the command 
	 * and place file.txt into the buffer file   */
	parse_request_get_file(thread_handle->request, thread_handle->file, thread_handle->file_size);

	int fd = open(file_path, O_RDONLY);
	if (fd == -1) return ERROR_FILE;

	thread_handle->read_bytes = read(fd, thread_handle->reply, thread_handle->reply_size);
	if (read_bytes == -1) {
		return ERROR_BYTES;
	}

	close(fd);

	thread_handle->reply[thread_handle->read_bytes] = '\0';
	return SUCCESS;
}

int command_handle_list(thread_handler_t *thread_handle)
{
	/* Opens the directory pointed by argv[3] if it is 
	 * unable to open the directory return of ER_DIR */
	DIR *dir_fd = opendir(thread_handle->directory);
	if (!dir_fd) 
	{
		error_msg("ER_DIR\n");
		return ERROR_DIR;
	}

	/* Overflow keeps a track of amount of bytes that are being
	 * added into when cating data into buffer               */
	size_t overflow_meter = 0;

	struct dirent *dir;
	while ((dir = readdir(dir_fd)))
	{
		if (!strcmp(dir->d_name, ".") || 
		    !strcmp(dir->d_name, ".."))
			continue;

		/* Addes to the overflow meter and comparies it to the
		   reply_size , if it passes it will stop and close */
		overflow_meter += strlen(dir->d_name) + strlen("\n");
		if (thread_handle->reply_size <= overflow_meter)
		{
			error_msg("Overflow\n");
			closedir(dir_fd);
			return ERROR_OVERFLOW;
		} 
		strcat(thread_handle->reply, dir->d_name);
		strcat(thread_handle->reply, "\n");
	}

	closedir(dir_fd);

	return SUCCESS;
}

void command_handle_invalid(thread_handler_t *thread_handle)
{
	char *invalid_command = parse_request_get_command(thread_handle->request);

	snprintf(thread_handle->reply,thread_handle->reply_size,"Status: Bad\nRequest: %s\n",invalid_command);
}
