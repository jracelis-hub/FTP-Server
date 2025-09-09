#include "commands.h"
#include "cmd_parsing.h"
#include "net_utility.h"
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

int get_command(int client_fd,char *request,size_t request_s) {

	char *command_types[] = {"Download;",
	                          "Upload;",
							  "List;",
							  "Read;"};

	ssize_t recv_bytes = recv(client_fd,request,request_s,0);
	if ( recv_bytes < 0 ) {
		return ER_BYTES;
	}

	printf("Received: %zu bytes\n",recv_bytes);
	printf("Request: %s",request);
	
	if (strncmp(request,command_types[0],strlen(command_types[0])) == 0) {
		return DOWNLOAD;
	} else if (strncmp(request,command_types[1],strlen(command_types[1])) == 0) {
		return UPLOAD;
	} else if (strncmp(request,command_types[2],strlen(command_types[2])) == 0) {
		return LIST;
	} else if (strncmp(request,command_types[3],strlen(command_types[3])) == 0) {
		return READ;
	} else {
		return ER_CMD;
	}
}

int list_cmd(int client_fd,char *reply,size_t reply_s,char *dir) {
		
	DIR *d_fd = opendir(dir);
	if (!d_fd) {
		return ER_FILE;
	}

	char temp[BUFFER];
	memset(temp,0,sizeof(temp));

	struct dirent *entity;
	entity = readdir(d_fd);

	while (entity != NULL) {
		if (strcmp(entity->d_name,".") == 0 || 
		    strcmp(entity->d_name,"..") == 0)
			continue;
		strncat(temp,entity->d_name,sizeof(temp) - strlen(temp) - 1);
		strncat(temp,"\n",sizeof(temp) - strlen(temp) - 1);
		entity = readdir(d_fd);
	}

	/* Free the directory pointer */
	closedir(d_fd);

	snprintf(reply,reply_s,"%s\r\n",temp);

	ssize_t bytes = send(client_fd,reply,strnlen(reply,reply_s),0);
	if ( bytes < 0 ) {
		return ER_BYTES;
	}

	return 0;
}

void send_close_connection(int client_fd,char *request,char *reply,size_t reply_s) {
	
	const char *exit_status = "Status: Invalid\r\n";
	char *command = parse_request(request);

	snprintf(reply,reply_s,"Request: %s\n%s",command,exit_status);

	ssize_t send_bytes = send(client_fd,reply,strnlen(reply,reply_s),0);
	if ( send_bytes < 0 ) {
		error_msg("Could not send exit message");
		return;
	}
}
