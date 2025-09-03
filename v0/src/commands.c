#include "commands.h"
#include "net_utility.h"
#include <dirent.h>

int server_welcome_msg(int *client_fd,ssize_t *bytes,const char *welcome_msg) {

	*bytes = send(*client_fd,welcome_msg,strlen(welcome_msg),0);
	if ( *bytes < 0 ) { return -1; }
	return 0;
}

int print_client_ip(struct sockaddr_in *client_in,socklen_t *client_len) {

	/* Macros needed to hold 
	   IPv4 & Port size   */
	char host_ip[NI_MAXHOST];
	socklen_t host_ip_len;
	char host_port[NI_MAXSERV];
	socklen_t host_port_len;

	host_ip_len = sizeof(host_ip);
	host_port_len = sizeof(host_port);
	if ( getnameinfo((struct sockaddr*)client_in,*client_len,
					 host_ip, host_ip_len,
					 host_port, host_port_len,
					 NI_NUMERICHOST | NI_NUMERICSERV) != 0 ) { return -1; }

	/* Once accepted print out IPv4:Port that was accepted on server side */
	printf("Accepted connection from %s:%s\n",host_ip,host_port);

	return 0;
}

void list_cmd(char *response,char **dir_name) {
	
	DIR *dir_fd;
	struct dirent *directory;

	/* Open directory to read from */
	dir_fd = opendir(dir_name[3]);
	if ( dir_fd == NULL ) {
		error_msg("Could not open directory");
	}

	/* Read each file name that is in the directory not including . .. or .dotfiles */
	while ( (directory = readdir(dir_fd)) != NULL ) {
		if ( strcmp(directory->d_name,".") == 0 || 
		     strcmp(directory->d_name,"..") == 0 ||
			 strncmp(directory->d_name,".",1) == 0) { continue; }

		strncat(response,directory->d_name,256);
		strcat(response,"\n");
	}

	/* Close the open directory descriptor */
	closedir(dir_fd);
} 

/* TODO
void download_cmd(char *file_name) {
	int fd;

	fd = open(""
	
}
*/

int check_cmd(int *client_fd,ssize_t *bytes,char *receive) {

	*bytes = recv(*client_fd,receive,sizeof(receive),0);
	if ( *bytes == -1 ) {
		error_msg("Could not receive bytes from client");
	}

	printf("Bytes received: %zu\n",*bytes);
	printf("Request: %s",receive);

	/* The request from client will compare to one of these 4 commands
	   if the results are not 0 then it will return a fail -1 */
	char *cmd[] = { "Download\n","Upload\n","List\n","Read\n" };

	if (!strcmp(receive,cmd[0])) {
		return DOWNLOAD;
	} else if (!strcmp(receive,cmd[1])) {
		return UPLOAD;
	} else if (!strcmp(receive,cmd[2])) {
		return LIST;
	} else if (!strcmp(receive,cmd[2])) {
		return READ;
	} else {
		return -1;
	}
}

int do_cmd(int status,int *client_fd,ssize_t *bytes,char *response,char **dir_name) {

	char *statuses[] = {"Request: Download\nStatus: Okay\nFile: random.txt\n",
                        "Request: Upload \nStatus: Okay\nFile: random.txt\n",
					    "Request: List \nStatus: Okay\nFiles:\n",
					    "Request: Read \nStatus: Okay\nFile:\n" };

		if ( status == DOWNLOAD ) {
			strncpy(response,statuses[0],BUFFER);
			
			*bytes = send(*client_fd,response,strnlen(response,BUFFER),0);
			if ( *bytes == -1 ) {
				error_msg("Unable to send a response to client");
				return -1;
			}
		} else if ( status == UPLOAD ) {
			strncpy(response,statuses[1],BUFFER);

			*bytes = send(*client_fd,response,strnlen(response,BUFFER),0);
			if ( *bytes == -1 ) {
				error_msg("Unable to send a response to client");
				return -1;
			}
		} else if ( status == LIST ) { 

			strncpy(response,statuses[2],BUFFER);

			/* Preform the list command */
			list_cmd(response,dir_name);

			*bytes = send(*client_fd,response,strnlen(response,BUFFER),0);
			if ( *bytes == -1 ) {
				error_msg("Unable to send a response to client");
				return -1;
			}
		} else if ( status == READ ) {
			strncpy(response,statuses[3],BUFFER);
			*bytes = send(*client_fd,response,strnlen(response,BUFFER),0);
			if ( *bytes == -1 ) {
				error_msg("Unable to send a response to client");
				return -1;
			}
		}
		return 0;
}
