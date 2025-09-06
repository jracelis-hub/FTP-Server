#include "server_utility.h"
#include "net_utility.h"

int server_welcome_msg(int client_fd) {
	
	/* Initial message to be sent to client after connection */
	static const char *welcome_msg = 
	"-------------------------------\n"
	"|  Welcome to RPi FTP Server  |\n"
	"|-----------------------------|\n"
	"|  Use the following methods: |\n"
	"|------------------------------\n"
	"|  Download a file         |\n"
	"|  Upload a file           |\n"
	"|  List files in directory |\n"
	"|  Read file content       |\n"
	"----------------------------\n";
	ssize_t bytes;

	/* Ensure to plus 1 on strlen as it does not include NULL terminator */
	bytes = send(client_fd,welcome_msg,strlen(welcome_msg)+1,0);
	if ( bytes < 0 ) { return ERROR; }

	return 0;
}

int print_client_ip(char *host_ip,socklen_t host_ip_len,char *host_port,socklen_t host_port_len,struct sockaddr_in *client_in,socklen_t *client_len) {

	if ( getnameinfo((struct sockaddr*)client_in,*client_len,
					 host_ip, host_ip_len,
					 host_port, host_port_len,
					 NI_NUMERICHOST | NI_NUMERICSERV) != 0 ) { return ERROR; }

	/* Once accepted print out IPv4:Port that was accepted on server side */
	printf("Accepted connection from %s:%s\n",host_ip,host_port);

	return 0;
}

void disconnect_host(char *host_ip,char *host_port ) {
	fprintf(stderr,"Disconnecting from %s:%s\n",host_ip,host_port);
}
