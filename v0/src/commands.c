#include "commands.h"
#include "net_utility.h"
#include "command_utility.h"
#include <dirent.h>
#include <fcntl.h>

int check_cmd(int client_fd,char *receive,size_t receive_buffer) {

	ssize_t bytes = recv(client_fd,receive,receive_buffer,0);
	if ( bytes == ERROR ) {
		error_msg("Could not receive bytes from client"); return ER_BYTES; }

	printf("Bytes received: %zu\n",bytes);
	printf("Request: %s",receive);

	/* The request from client will compare to one of these 4 commands
	   if the results are not one of the macros then it will return a 
	   fail -1                                                      */
	char *cmd[] = {"Download;",
	               "Upload;",
				   "List;",
				   "Read;"};

	if (!strncmp(receive,cmd[0],strlen(cmd[0]))) {
		return DOWNLOAD;
	} else if (!strncmp(receive,cmd[1],strlen(cmd[1]))) {
		return UPLOAD;
	} else if (!strncmp(receive,cmd[2],strlen(cmd[2]))) {
		return LIST;
	} else if (!strncmp(receive,cmd[3],strlen(cmd[3]))) {
		return READ;
	} else {
		receive[bytes-1] = '\0';
		return ERROR;
	}
}

int do_cmd(int cmd_status,int client_fd,char *response,size_t response_buffer,char *receive,char **dir_name) {

	int status;

	/* The following variables are needed to parse the command */
	const int file_buff = 128;
	const int request_buff = 32;
	char request[request_buff];
	char file[file_buff];

	parse_cmd(file,sizeof(file),request,sizeof(request),receive);

	if ( cmd_status == DOWNLOAD ) {
		status = rod_cmd(client_fd,file,request,response,response_buffer,dir_name);
		if ( status == ERROR ) {
			return ERROR;
		} if ( status == ER_BYTES ) {
			return ERROR;
		}

	} 
	else if ( cmd_status == UPLOAD ) {

	} 
	else if ( cmd_status == LIST ) { 

		/* Preform the list command */
		status = list_cmd(client_fd,request,response,response_buffer,dir_name);
		if ( status == ER_OVERFLOW ) {
			return ER_OVERFLOW;
		} else if ( status == ERROR ) {
			return ERROR;
		}
	} 
	else if ( cmd_status == READ ) {

		/* Preform the reading command */
		status = rod_cmd(client_fd,file,request,response,response_buffer,dir_name);
		if ( status == ERROR ) {
			return ERROR;
		} if ( status == ER_BYTES) {
			return ER_BYTES;
		}
	}

	return 0;
}

int rod_cmd(int client_fd,char *file,char *request,char *response,size_t response_len,char **dir_name) {

	/* To hold the directory+name */
	char file_read[256];
	/* This is to store temporary bytes before sending to socket */
	char temp_buffer[BUFFER];

	long overflow_meter = BUFFER;

	memset(temp_buffer,0,BUFFER);
	
	snprintf(file_read,sizeof(file_read),"%s/%s",dir_name[3],file);
	int fd = open(file_read,O_RDONLY);
	if ( fd == ERROR ) {
		error_msg("Could not open file");
		return ER_MISFILE;
	}

	lseek(fd,0,SEEK_SET);

	overflow_meter -= strlen(file);

	if (strcmp(request,"Read") == 0 ) {

		const char *read_header = "Request: Read\nStatus: Okay\nFile: ";
		const char *format = "\n\r\n";
		
		overflow_meter -= strlen(format);
		overflow_meter -= strlen(read_header);

		ssize_t read_bytes = read(fd,temp_buffer,BUFFER);
		overflow_meter -= read_bytes;
		if ( read_bytes < 0 || overflow_meter < 1) {
			return ER_BYTES;
		}
		
		printf("%zu\n",read_bytes);

		snprintf(response,response_len,"%s%s\n%s\r\n",read_header,file,temp_buffer);

		ssize_t send_bytes = send(client_fd,response,strlen(response),0);
		if ( send_bytes < 0 ) {
			return ER_BYTES;
		}
	} else if ( strcmp(request,"Download") == 0 ) {

		const char *download_header = "Request: Download\nStatus: Okay\nFile:";
		const char *format = "\r\n";

		overflow_meter -= strlen(format);
		overflow_meter -= strlen(download_header);

		ssize_t read_bytes = read(fd,temp_buffer,BUFFER);
		if ( read_bytes < 0 || overflow_meter < 1) {
			return ER_BYTES;
		}
		
		printf("%zu\n",read_bytes);

		snprintf(response,response_len,"%s%s\n%s\r\n",download_header,file,temp_buffer);

		ssize_t send_bytes = send(client_fd,response,strlen(response),0);
		if ( send_bytes < 0 ) {
			return ER_BYTES;
		}
	}

	close(fd);

	return 0;
}

int list_cmd(int client_fd,char *request,char *response,size_t response_buffer,char **dir_name) {
	
	char list_status[] = "Request: List;\nStatus: Okay\nFiles:\n";

	DIR *dir_fd;
	struct dirent *directory;
	ssize_t bytes;

	const int buffer = 4056;
	char temp_buffer[buffer];
	size_t buffer_cap = 0;

	memset(temp_buffer,0,sizeof(temp_buffer));
	buffer_cap = strlen(list_status);
	
	/* Open directory to read from */
	dir_fd = opendir(dir_name[3]);
	if ( dir_fd == NULL ) {
		error_msg("Could not open directory");
		return ER_MISFILE;
	}

	/* Read each file name that is in the directory not including . .. or .dotfiles */
	while ( (directory = readdir(dir_fd)) != NULL ) {
		if ( strcmp(directory->d_name,".") == 0 || 
		     strcmp(directory->d_name,"..") == 0 ||
			 strncmp(directory->d_name,".",1) == 0) { continue; }

		buffer_cap += strlen(directory->d_name) + 1;
		if ( buffer_cap >= response_buffer ) {
			error_msg("Buffer overflow");
			return ER_OVERFLOW;

		}

		strcat(temp_buffer,directory->d_name);
		strcat(temp_buffer,"\n");
	}

	/* Close the open directory descriptor */
	closedir(dir_fd);

	printf("Response buffer = %ld\n",response_buffer - buffer_cap);
	printf("Buffer cap = %ld\n",buffer_cap);
	
	snprintf(response,response_buffer,"%s%s",list_status,temp_buffer);
	bytes = send(client_fd,response,strnlen(response,response_buffer)+1,0);
	if ( bytes == -1 ) {
		error_msg("Unable to send a response to client");
		return ER_BYTES;
	}

	return 0;
} 

void send_exit_cmd(int client_fd,char *receive,char *response,size_t response_buffer) {

	ssize_t bytes;
	static const char *disconnect_msg = "Disconnecting from server...";
	static const char *invalid = "Status: Invalid";
	static const char *exit = "\r\n";
	
	snprintf(response,response_buffer,"Request: %s\n%s\n%s%s"
	                                  ,receive,invalid,disconnect_msg,exit);
	bytes = send(client_fd,response,strnlen(response,response_buffer)+1,0);
	if ( bytes == -1 ) {
		return;
	}

	if ( receive == NULL ) {
		snprintf(response,response_buffer,"%s",exit);
		bytes = send(client_fd,response,strnlen(response,response_buffer)+1,0);
		if ( bytes == -1 ) {
			return;
		}
	}
}

