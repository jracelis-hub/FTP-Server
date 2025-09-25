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

int command_get_file(char *request, char *file, size_t size)
{
	char *p;
	int i = 0;
	for (p = request; *p != '\0'; p++)
	{
		/* The following looks for a ; / or a ' ' if the following is met
		 * the pointer to request will be set after it 
		 * Example: Download; /path/file.txt\n
		 * once Download; request is now pointing to _/path/file.txt
		 * until is is left with file.txt                          */
		if (isasemicolon(*p) || isaslash(*p) || isaspace(*p))
		{
			request = p;
			request++;
		/* Once going through the whole request and 
		 * a newline is found it breaks          */
		}
		if (isanewline(*p))
			break;
	}

	/* Starting from the pointed location set from the for loop above 
	 * each character is placed into the array that was passed into         
	 * file[0] = f
	 * file[1] = i
	 * file[2] = l 
	 * and so on until the newline is met */
	while (*request != '\0' && *request != '\n')
	{
		if (i == size - 1)
			return ERROR_OVERFLOW;
		file[i++] = *request++;
	}
	file[i] = '\0';
	return SUCCESS;
}

char *command_get_request(char *command)
{
	char *p;
	for (p = command; *p != '\0'; p++)
	{
		if (isasemicolon(*p))
			*(p + 1) = '\0';
		else if (isanewline(*p))
			*p = '\0';
	}
	return command;
}

char *command_strip_header(char *payload)
{
	char *p;
	for (p = payload; *p != '\n'; p++)
	{
		if (isanewline(*p))
		{
			payload = p;
			*p = '\0';
			payload++;
			return payload;
		}
	}
	return NULL;
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

int command_handle_read(char *request, char *reply, size_t reply_size, char *directory)
{
	/* File_path will hold the value of the directory (argv[2]) 
	 * + the file that wants to be read from the client 
	 * /home/something/filemention.txt                       */
	char file_path[256] = {0};
	size_t file_path_len = sizeof(file_path);

	/* File will hold the value of file requested from the client */
	char file[64] = {0}; 

	/* Command strip path will take the request:
	 * Read; file.txt and strip off the request 
	 * and place file.txt into the buffer file   */
	command_get_file(request, file, sizeof(file));

	/* isdirectoryformat checks if there is an appending / at the end
	 * of the * directory if not a / is appended to the end        */
	if (!isdirectoryformat(file_path))
		snprintf(file_path, file_path_len, "%s/%s", directory, file);
	else
		snprintf(file_path, file_path_len, "%s%s", directory, file);

	int fd = open(file_path, O_RDONLY);
	if (fd == -1) return ERROR_FILE;

	ssize_t read_bytes = read(fd, reply, reply_size - 1);
	if (read_bytes == -1) return ERROR_BYTES;
	else if (read_bytes > 0) reply[read_bytes] = '\0';

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

		/* Addes to the overflow meter and comparies it to the
		   reply_size (1024), if it passes it will stop and close */
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
	char *invalid_command = command_get_request(thread_handle->request);

	snprintf(thread_handle->reply,thread_handle->reply_size,"Status: Bad\nRequest: %s\n",invalid_command);
}
