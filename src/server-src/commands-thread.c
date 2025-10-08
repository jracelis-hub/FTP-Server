#include "commands-thread.h" 
#include "parsing.h"
#include "logging.h"
#include "thread.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

/* Checks for proper formatting from the request */
void command_receive_from_client(thread_handler_t *thread_handle)
{
	if (!strncmp(thread_handle->request, "Download;", strlen("Download;"))) 
	{ thread_handle->command = DOWNLOAD; }
	else if (!strncmp(thread_handle->request, "Upload;", strlen("Upload;"))) 
	{ thread_handle->command = UPLOAD; }
	else if (!strncmp(thread_handle->request, "List;", strlen("List;"))) 
	{ thread_handle->command = LIST; }
	else if (!strncmp(thread_handle->request, "Read;", strlen("Read;"))) 
	{ thread_handle->command = READ; }
	else 
	{ thread_handle->command = INVALID; }
}

/* Handles all the command types */
int command_handler(thread_handler_t *thread_handle)
{
	/* Error codes are handled in the command handle itself 
	 * if the command is INVALID/default it will perform
	 * command handle invalid                           */
	switch (thread_handle->command) 
	{
		case DOWNLOAD:
			if (command_handle_download(thread_handle) < 0 ) 
			{ return ERROR_COMMAND; }
			break;
		case UPLOAD:
			if (command_handle_upload(thread_handle) < 0 ) 
			{ return ERROR_COMMAND; }
			break;
		case LIST:
			if (command_handle_list(thread_handle) < 0) 
			{ return ERROR_COMMAND; }
			break;
		case READ:
			if (command_handle_read(thread_handle) < 0) 
			{ return ERROR_COMMAND; }
			break;
		case INVALID:
			command_handle_invalid(thread_handle);
			return ERROR_COMMAND;
		default:
			command_handle_invalid(thread_handle);
			return ERROR_COMMAND;
	}
	return SUCCESS;
}

int command_handle_download(thread_handler_t *thread_handle)
{
	int fd;
	ssize_t read_bytes;

	/* Gets the file part of the request that was sent from the client */
	if (parse_request_get_file(thread_handle->request, thread_handle->file, 
	                           thread_handle->file_size) < 0) 
	{
		error_msg("File length to long for file buffer");
		return ERROR_OVERFLOW;
	}
	
	/* Checks if there is enough space to hold directory path and file */
	if (strlen(thread_handle->directory) + strlen(thread_handle->file) + 1
	    > thread_handle->directory_size) 
	{
		error_msg("Directory buffer full");
		return ERROR_OVERFLOW;
	}

	/* /home/directory/ + file.txt */
	strcat(thread_handle->directory, thread_handle->file);
	
	fd = open(thread_handle->directory, O_RDONLY);
	if (fd < 0) 
	{
		error_msg("Could not open file");
		return ERROR_FILE;
	}

	read_bytes = read(fd, thread_handle->reply, 
	                          thread_handle->reply_size);
	if (thread_handle->reply_size == read_bytes) 
	{
		error_msg("Reading bytes in buffer overflow");
		close(fd);
		return ERROR_OVERFLOW;
	}
	
	close(fd);

	return SUCCESS;
}

int command_handle_upload(thread_handler_t *thread_handle) 
{
	int fd;
	char *payload = NULL;
	ssize_t write_bytes;

	/* Get the file from request */
	if (parse_request_get_file(thread_handle->request, thread_handle->file, 
	                           thread_handle->file_size) < 0) 
	{
		error_msg("File length to long for file buffer");
		return ERROR_OVERFLOW;
	}
	
	/* Checks if length of directory and file + '\0' is greater than directory size */
	if (strlen(thread_handle->directory) + strlen(thread_handle->file) + 1 
	    > thread_handle->directory_size) 
	{
		error_msg("Directory buffer full");
		return ERROR_OVERFLOW;
	}

	/* Copies /directory/file.txt 
	 * Note: directory is already appended with a / see 
	 * server_set_directory                         */
	strcat(thread_handle->directory, thread_handle->file);

	/* Creating file with user mode of write and read permission */
	fd = open(thread_handle->directory, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
	if (fd < 0) 
	{
		error_msg("Could not create the file on server");	
		return ERROR_FILE;
	}

	/* Return a pointer after the request, capturing the payload */
	payload = parse_request_get_payload(thread_handle->request);
	if (!payload) 
	{
		error_msg("Unable to parse request and get payload");
		return ERROR_FILE;
	}

	/* Write the payload buffer into the new file on server */
	write_bytes = write(fd, payload, strlen(payload));
	if (write_bytes < 0) 
	{
		error_msg("Error could not write bytes to the file");
		close(fd);
		return ERROR_BYTES;
	}

	close(fd);
	return SUCCESS;
}

int command_handle_list(thread_handler_t *thread_handle)
{
	/* Overflow keeps a track of bytes that are being
	 * added into buffer after cating              */
	size_t overflow_meter = 0;
	DIR *dir_fd = NULL;
	struct dirent *dir = NULL;

	/* Opens the directory pointed by argv[3] if it is 
	 * unable to open the directory return of ER_DIR */
	dir_fd = opendir(thread_handle->directory);
	if (!dir_fd) 
	{
		error_msg("Could not open directory");
		return ERROR_DIR;
	}

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
			error_msg("Directory size too big, buffer overflow");
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
	int fd;
	ssize_t read_bytes;

	/* Get the file from request */
	if (parse_request_get_file(thread_handle->request, thread_handle->file, 
	                           thread_handle->file_size) < 0) 
	{
		error_msg("File length to long for file buffer");
		return ERROR_OVERFLOW;
	}
	
	/* Checks if length of directory and file + '\0' is greater than directory size */
	if (strlen(thread_handle->directory) + strlen(thread_handle->file) + 1 
	    > thread_handle->directory_size) 
	{
		error_msg("Directory buffer full");
		return ERROR_OVERFLOW;
	}

	/* Copies /directory/file.txt 
	 * Note: directory is already appended with a / see 
	 * server_set_directory                         */
	strcat(thread_handle->directory, thread_handle->file);

	fd = open(thread_handle->directory, O_RDONLY);
	if (fd < 0) 
	{
		error_msg("Could not open file");
		return ERROR_FILE;
	}

	read_bytes = read(fd, thread_handle->reply, 
	                          thread_handle->reply_size);
	if (thread_handle->reply_size == read_bytes) 
	{
		error_msg("Reading bytes in buffer overflow");
		close(fd);
		return ERROR_OVERFLOW;
	}
	
	close(fd);

	return SUCCESS;
}

void command_handle_invalid(thread_handler_t *thread_handle)
{
	char *invalid_command = NULL;
	invalid_command = parse_request_get_command(thread_handle->request);

	snprintf(thread_handle->reply,thread_handle->reply_size,"Status: Bad\nRequest: %s\n", invalid_command);
}
