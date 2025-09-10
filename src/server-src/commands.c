#include "commands.h"
#include "cmd_parsing.h"
#include "net_utility.h"
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

int receive_command(cmd_t *cmd) {
	
	#if defined(DEBUG)
		printf("[DEBUG] Client FD = %d\n",cmd->sock_fd);
		printf("[DEBUG] Size of buffer = %lu\n",cmd->request_s);
	#endif
	char *command_check[] = { "Download;","Upload;","List;","Read;" };

	ssize_t bytes = recv(cmd->sock_fd,cmd->request,cmd->request_s,0);
	if ( bytes < 0 ) {
		return ER_BYTES;
	}

	printf("Request: %s",cmd->request);
	#if defined(DEBUG)
		printf("[DEBUG] Received %zu bytes\n",bytes);
		for (int i=0;i<4;i++)
			printf("[DEBUG] Size of %s = %ld bytes\n",command_check[i],strlen(command_check[i]));
	#endif
	
	if (!strncmp(cmd->request,command_check[0],strlen(command_check[0]))) {
		return DOWNLOAD;
	} else if (!strncmp(cmd->request,command_check[1],strlen(command_check[1]))) {
		return UPLOAD;
	} else if (!strncmp(cmd->request,command_check[2],strlen(command_check[2]))) {
		return LIST;
	} else if (!strncmp(cmd->request,command_check[3],strlen(command_check[3]))) {
		return READ;
	} else {
		return ER_CMD;
	}
}

int do_command(int command,cmd_t *cmd) {
	
	#if defined(DEBUG)
		printf("[DEBUG] Command value = %d\n",command);
		printf("[DEBUG] Directory = %s\n",cmd->directory);
	#endif
	int status;

	int (*command_handler[4])(cmd_t *cmd) = {
		download_command,
		upload_command,
		list_command,
		read_command,
	};

	status = command_handler[command](cmd);
	if ( status < 0 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Status code = %d\n",status);
	#endif
		return -1;
	}

	return 0;
}

int download_command(cmd_t *cmd) {
	printf("%d",cmd->sock_fd);
	return 0;
}

int upload_command(cmd_t *cmd) {
	printf("%d",cmd->sock_fd);
	return 0;
}

int list_command(cmd_t *cmd) {

	char temp[BUFFER] = {0};
	long buffer_counter = BUFFER - 1;

	DIR *d_fd;
	d_fd = opendir(cmd->directory);
	if (!d_fd) {
		return ER_FILE;
	}

	struct dirent *entity;

	while( (entity=readdir(d_fd)) != NULL) {
		if (!strcmp(entity->d_name,".") ||
		    !strcmp(entity->d_name,".."))
		{ continue; }
		if ( buffer_counter < 0 ) {
			break;
		}
		strcat(temp,entity->d_name);
		strcat(temp,"\n");
		buffer_counter -= strlen(entity->d_name) - 1;
	}

	if ( buffer_counter < 0 ) {
		return ER_OVERFLOW;
	}

	#if defined(DEBUG)
		printf("[DEBUG] Buffer Leftover = %ld\n",buffer_counter);
	#endif

	closedir(d_fd);

	snprintf(cmd->reply,cmd->reply_s,"%s",temp);
	ssize_t bytes = send(cmd->sock_fd,cmd->reply,strlen(cmd->reply),0);
	if ( bytes < 0 ) {
		return ER_BYTES;
	}
	
	return 0;
}

int read_command(cmd_t *cmd) {

	char temp[BUFFER] = {0};
	char file_name[256] = {0};

	char *file = parse_file(cmd->request);
	if ( file == NULL ) {
		return ER_PARSE;
	}

	snprintf(file_name,256,"%s/%s",cmd->directory,file);
	#if defined(DEBUG)
		printf("[DEBUG] File name = %s\n",file_name);
	#endif 

	int fd = open(file_name,O_RDONLY);
	if ( fd == -1 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Could not open file\n");
	#endif 
		return ER_FILE;
	}
	
	ssize_t read_bytes = read(fd,temp,sizeof(temp));
	if ( read_bytes < 0 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Read bytes buffer overflow\n");
	#endif 
		return ER_BYTES;
	}

	close(fd);

	snprintf(cmd->reply,cmd->reply_s,"%s\n",temp);

	ssize_t send_bytes = send(cmd->sock_fd,cmd->reply,strlen(cmd->reply),0);
	if ( send_bytes < 0 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Send bytes buffer overflow\n");
	#endif 
		return ER_BYTES;
	}
	return 0;
}
