#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>                 /* Used for socket */
#include <netinet/in.h>                 /* Used for sockaddr_in */
#include <arpa/inet.h>
#include <netdb.h>
#include "net_utility.h"
#include "serv_utility.h"
#include "commands.h"
#include "cmd_parsing.h"

#define MAX_CONN 10
#define BUFFER 4056

int main(int argc, char *argv[]) {

	/* Requres 3 arguments */
	if ( argc < 4 ) {
		usage(argv);
		return -1;
	}

	int port = atoi(argv[2]);

	if ( port < 1024 ) {
		error_msg("Port has to be above 1023");
		usage(argv);
		return -1;
	}
	else if ( port > 65535 ) {
		error_msg("Port has to be below 65535");
		usage(argv);	
		return -1;
	}

	/* Client information */
	int client_fd;
	struct sockaddr_in client;
	socklen_t client_s = sizeof(client);

	/* Send and receive buffer */
	char request[BUFFER];
	char reply[BUFFER];

	int command;

	/* Get process ID of the current process */
	pid_t process_id = getpid();
	perform_task("Created Process:",&process_id);

	/* Getting protocol TCP */
	const char *proto_name = "tcp";
	struct protoent *proto = getprotobyname(proto_name);
	if (!proto) {
		error_msg("Could not get protocol name");
		return -1;
	}

	/* Creating Listening Socket */
	perform_task("Creating listening socket...",NULL);
	int listen_fd = socket(AF_INET,SOCK_STREAM,proto->p_proto);
	if ( listen_fd < 0 ) {
		error_msg("Could not create listening socket");
		return -1;
	}

	/* Creating socket structure to hold IPv4 and Port 
	   Ensure structure is clear                    */
	struct sockaddr_in server_in;
	memset(&server_in,0,sizeof(struct sockaddr_in));
	server_in.sin_family = AF_INET;
	server_in.sin_port = htons(port);
	int status = inet_pton(AF_INET,argv[1],&server_in.sin_addr);
	if ( status != 1 ) {
		error_msg("Could not convert network address");
		return -1;
	}

	pass_msg(NULL,NULL);

	perform_task("Binding listening socket to port and address...",NULL);
	status = bind(listen_fd,(struct sockaddr *)&server_in,sizeof(server_in));
	if ( status != 0 ) {
		error_msg("Could not bind socket with port and address");
		clean_up(&process_id,&listen_fd,NULL);
		return -1;
	}

	pass_msg(NULL,NULL);

	perform_task("Waiting for incoming connections...",NULL);
	status = listen(listen_fd,MAX_CONN);
	if ( status != 0 ) {
		error_msg("Could not set up listening socket");
		clean_up(&process_id,&listen_fd,NULL);
		return -1;
	}

	pass_msg(NULL,NULL);

	int done = 0;

	while(1) {
	
		memset(request,0,sizeof(request));
		memset(reply,0,sizeof(reply));

		if (!done) {
			
			perform_task("Attempting to accept connection...",NULL);
			client_fd = accept(listen_fd,(struct sockaddr *)&client,&client_s);
			if ( client_fd < 0 ) {
				error_msg("Could not accept connection");
			}

			status = print_ip_port(&client,sizeof(client));
			if ( status == -1 ) {
				error_msg("Could not get address and port of client");
				clean_up(NULL,NULL,&client_fd);
			}

			status = welcome_msg(client_fd);
			if ( status == -1 ) {
				error_msg("Could not send welcome message");
				clean_up(NULL,NULL,&client_fd);
			}

			done = 1;
		}

		command = get_command(client_fd,request,sizeof(request));
		if ( command != DOWNLOAD && command != UPLOAD && command && READ && command != LIST ) {
			error_msg("Could not get command");	
			send_close_connection(client_fd,request,reply,sizeof(reply));
			clean_up(NULL,NULL,&client_fd);
			done = 0;
		}

		switch (command) {
			case DOWNLOAD:
			case UPLOAD:
			case READ:
				char *file = parse_file(request);
				if ( file == NULL ) {
					error_msg("Could parse file");
					clean_up(NULL,NULL,&client_fd);
					done = 0;
				}
				break;
			case LIST:
				status = list_cmd(client_fd,reply,sizeof(reply),argv[3]);	
				if ( status < 0 ) {
					error_msg("Could not preform list command");	
					clean_up(NULL,NULL,&client_fd);
					done = 0;
				}
				break;
			default:
				error_msg("Invalid command");
				clean_up(NULL,NULL,&client_fd);
				done = 0;
		}

	}

	clean_up(&process_id,&listen_fd,NULL);

	return 0;
}
