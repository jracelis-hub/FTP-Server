#include "interpret_command.h"
#include "net_utility.h"
#include <unistd.h>
#include <fcntl.h>

int interpret_cmd(int sock_fd,char *receive,size_t receive_s) {
	
	ssize_t bytes = recv(sock_fd,receive,receive_s,0);
	if ( bytes == -1 ) {
		return -1;
	}

	int status;
	const int file_buff = 128;
	const int request_buff = 32;
	char request[request_buff];
	char file[file_buff];

	parse_response(file,sizeof(file),request,sizeof(request),receive);

	if ( strcmp(request,"Download") == 0 ) {
		status = interpret_download(receive,bytes,file);
		if ( status != 0 ) {
			return -1;
		}
	}
	
	return 0;
}

int interpret_download(char *receive,ssize_t bytes,char *file) {

	int fd = open(file, O_CREAT | O_WRONLY,S_IRWXU);
	if ( fd < 0 ) {
		return -1;
	}

	ssize_t write_bytes = write(fd,receive,(size_t)bytes-2);
	if ( write_bytes < 0 ) {
		return -1;
	}

	close(fd);
	return 0;

}

void parse_response(char *file,size_t file_s,char *request,size_t request_s,char *receive) {

	sscanf(receive,"Request: %[A-Za-z]\nStatus: Okay\nFile: %[A-Za-z._-]",request,file);
}
