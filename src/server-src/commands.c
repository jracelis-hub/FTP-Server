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
			if (command_handle_download(thread_handle) < 0 ) {
				return ERROR_COMMAND;
			}
			break;
		case UPLOAD:
			if (command_handle_upload(thread_handle) < 0 ) {
				return ERROR_COMMAND;
			}
			break;
		case LIST:
			if (command_handle_list(thread_handle) < 0) {
				return ERROR_COMMAND;
			}
			break;
		case READ:
			if (command_handle_read(thread_handle) < 0) {
				return ERROR_COMMAND;
			}
			break;
		case INVALID:
			command_handle_invalid(thread_handle);
			break;
	}
	return SUCCESS;
}

int command_handle_download(thread_handler_t *thread_handle)
{
	if (strlen(thread_handle->directory) >= FILE_PATH_SIZE) {
		error_msg("File buffer is too small to hold directory path");
		return ERROR_OVERFLOW;
	}
	strcpy(thread_handle->file, thread_handle->directory);
	if (parse_request_get_file(thread_handle->request, 
	    thread_handle->file, FILE_PATH_SIZE) != 0) {
		error_msg("Could not get file from request");
		return ERROR_FILE;
	}

	int fd = open(thread_handle->file, O_RDONLY);
	if (fd < 0) {
		error_msg("Could not open file");
		return ERROR_FILE;
	}

	ssize_t read_bytes = read(fd, thread_handle->reply, 
	                          thread_handle->reply_size);
	if (thread_handle->reply_size == read_bytes) {
		error_msg("Reading bytes in buffer overflow");
		close(fd);
		return ERROR_OVERFLOW;
	}
	
	close(fd);

	return SUCCESS;
}

int command_handle_upload(thread_handler_t *thread_handle) 
{
	if (strlen(thread_handle->directory) >= FILE_PATH_SIZE) {
		error_msg("File buffer is too small to hold directory path");
		return ERROR_OVERFLOW;
	}
	strcpy(thread_handle->file, thread_handle->directory);
	if (parse_request_get_file(thread_handle->request, 
	    thread_handle->file, FILE_PATH_SIZE) != 0) {
		error_msg("Could not get file from request");
		return ERROR_FILE;
	}

	/* Creating file with the mode of write and read permission */
	int fd = open(thread_handle->file, O_CREAT | S_IRUSR);
	if (fd < 0) {
		error_msg("Could not create the file on server");	
		return ERROR_FILE;
	}

	ssize_t write_bytes = write(fd, thread_handle->request, 
	                            strlen(thread_handle->request));
	
	if (write_bytes < 0) {
		error_msg("Error writing bytes into new file");
		close(fd);
		return ERROR_BYTES;
	}

	close(fd);
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

		/* Adds to the overflow meter and comparies it to the
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

int command_handle_read(thread_handler_t *thread_handle)
{
	printf("%s\n", thread_handle->directory);
	if (strlen(thread_handle->directory) >= FILE_PATH_SIZE) {
		error_msg("File buffer is too small to hold directory path");
		return ERROR_OVERFLOW;
	}
	strcpy(thread_handle->file, thread_handle->directory);
	if (parse_request_get_file(thread_handle->request, 
	    thread_handle->file, FILE_PATH_SIZE) != 0) {
		error_msg("Could not get file from request");
		return ERROR_FILE;
	}

	printf("%s\n", thread_handle->file);

	int fd = open(thread_handle->file, O_RDONLY);
	if (fd < 0) {
		error_msg("Could not open file");
		return ERROR_FILE;
	}

	ssize_t read_bytes = read(fd, thread_handle->reply, 
	                          thread_handle->reply_size);
	if (thread_handle->reply_size == read_bytes) {
		error_msg("Reading bytes in buffer overflow");
		close(fd);
		return ERROR_OVERFLOW;
	}
	
	close(fd);

	return SUCCESS;
}

void command_handle_invalid(thread_handler_t *thread_handle)
{
	char *invalid_command = parse_request_get_command(thread_handle->request);

	snprintf(thread_handle->reply,thread_handle->reply_size,"Status: Bad\nRequest: %s\n",invalid_command);
}
