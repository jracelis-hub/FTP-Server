#include "serv_utility.h"
#include <stdio.h>
#include <netdb.h>
#include <string.h>

int welcome_msg(int client_fd) {
	
	/* Initial message to be sent to client after connection */
	static const char *welcome_message = 
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
	bytes = send(client_fd,welcome_message,strlen(welcome_message),0);
	if ( bytes < 0 ) { return -1; }

	return 0;
}

int print_ip_port(char *clientip,size_t clientips,char *clientport,size_t clientports,struct sockaddr_storage *client,socklen_t client_s) {
	
	if (getnameinfo((struct sockaddr *)client,client_s,
	                 clientip,clientips,
					 clientport,clientports,
					 NI_NUMERICHOST | NI_NUMERICSERV)) { return -1; }
	
	/* Once accepted print out IPv4:Port that was accepted on server side */
	printf("Accepted connection from %s:%s\n",clientip,clientport);

	return 0;
}

void shutdown_msg(char *clientip,char *clientport) {

	printf("Closing connection with %s:%s\n",clientip,clientport);
}
