#include "serv_utility.h"
#include <stdio.h>
#include <netdb.h>
#include <string.h>

int welcome_msg(int client_fd) {
	
	/* Initial message to be sent to client after connection */
	static const char *welcome_msg = 
	"-------------------------------\n"
	"|  Welcome to RPi FTP Server  |\n"
	"|-----------------------------|\n"
	"|  Use the following methods: |\n"
	"|------------------------------\n"
	"|  Download; file   |\n"
	"|  Upload; file     |\n"
	"|  List;            |\n"
	"|  Read; file       |\n"
	"---------------------\n";
	ssize_t bytes;

	/* Ensure to plus 1 on strlen as it does not include NULL terminator */
	bytes = send(client_fd,welcome_msg,strlen(welcome_msg)+1,0);
	if ( bytes < 0 ) { return -1; }

	return 0;
}

int print_ip_port(struct sockaddr_in *client,socklen_t client_len) {
	
	char client_ip[NI_MAXHOST];
	char client_port[NI_MAXSERV];

	if (getnameinfo((struct sockaddr *)client,client_len,
	                 client_ip,sizeof(client_ip),
					 client_port,sizeof(client_port),
					 NI_NUMERICHOST | NI_NUMERICSERV)) { return -1; }
	
	/* Once accepted print out IPv4:Port that was accepted on server side */
	printf("Accepted connection from %s:%s\n",client_ip,client_port);

	return 0;
}
