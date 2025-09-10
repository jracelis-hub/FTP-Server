/* Standard Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Socket Headers */
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* User Define Headers */
#include "net_utility.h"
#include "serv_utility.h"
#include "commands.h"
#include "cmd_parsing.h"

#define MAX_CONN 10

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

#if defined(DEBUG) && !defined(INET)
	#define INET
	printf("[DEBUG] Debug mode enabled\n");
#endif
	
#if defined(INET)
	printf("IPv4 Server\n");
#elif defined(INET6)
	printf("IPv6 Server\n");
#endif
		
	/* Client information holds both IPv4 & IPv6 */
	struct sockaddr_storage client;
	socklen_t client_s = sizeof(client);

	/* Send and receive buffer */
	cmd_t *cmd = malloc(sizeof(cmd_t));
	if (!cmd) {
		error_msg("Could not allocate space for command structure");
		return -1;
	}
	/* Setting up the command structure values and 
	   ensure they are empty                    */
	memset(cmd,0,sizeof(cmd_t));
	cmd->reply_s = sizeof(cmd->reply);
	cmd->request_s = sizeof(cmd->request);
	strncpy(cmd->directory,argv[3],sizeof(cmd->directory));

	int command_type;
	int status;

	/* To hold IP and Port of client */
	char clientip[NI_MAXHOST];
	char clientport[NI_MAXSERV];

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
#if defined(INET)
	int listen_fd = socket(AF_INET,SOCK_STREAM,proto->p_proto);
#elif defined(INET6)
	int listen_fd = socket(AF_INET6,SOCK_STREAM,proto->p_proto);
#endif
	if ( listen_fd < 0 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Status code = %d",listen_fd);
	#endif
		error_msg("Could not create listening socket");
		return -1;
	}

#if defined(INET)
	/* Creating socket structure to hold IPv4 and Port 
	   Ensure structure is clear                    */
	struct sockaddr_in server_in;
	memset(&server_in,0,sizeof(struct sockaddr_in));
	server_in.sin_family = AF_INET;
	server_in.sin_port = htons(port);
	status = inet_pton(AF_INET,argv[1],&server_in.sin_addr);
	if ( status != 1 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Status code = %d\n",status);
	#endif
		error_msg("Could not convert ipv4 address");
		return -1;
	}
#elif defined(INET6)
	struct sockaddr_in6 server_in6;
	memset(&server_in6,0,sizeof(struct sockaddr_in6));
	server_in6.sin_family = AF_INET6;
	server_in6.sin_port = htons(port);
	status = inet_pton(AF_INET6,argv[1],&server_in6.sin_addr);
	if ( status != 1 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Status code = %d\n",status);
	#endif
		error_msg("Could not convert ipv6 address");
		return -1;
	}
#endif

	pass_msg(NULL,NULL);

	perform_task("Binding listening socket to port and address...",NULL);
#if defined(INET)
	status = bind(listen_fd,(struct sockaddr *)&server_in,sizeof(server_in));
#elif defined(INET6)
	status = bind(listen_fd,(struct sockaddr *)&server_in6,sizeof(server_in6));
#endif
	if ( status != 0 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Status code = %d\n",status);
	#endif
		error_msg("Could not bind socket with port and address");
		clean_up(&process_id,&listen_fd,NULL);
		return -1;
	}


	pass_msg(NULL,NULL);

	perform_task("Waiting for incoming connections...",NULL);
	status = listen(listen_fd,MAX_CONN);
	if ( status != 0 ) {
	#if defined(DEBUG)
		printf("[DEBUG] Status code = %d\n",status);
	#endif
		error_msg("Could not set up listening socket");
		clean_up(&process_id,&listen_fd,NULL);
		return -1;
	}

	pass_msg(NULL,NULL);

	int done = 0;

	while(1) {
	
		/* Ensure that everything in the buffer is zero'd out */
		memset(cmd->request,0,cmd->request_s);
		memset(cmd->reply,0,cmd->reply_s);

		if (!done) {
			
			perform_task("Attempting to accept connection...",NULL);
			cmd->sock_fd = accept(listen_fd,(struct sockaddr *)&client,&client_s);
		#if defined(DEBUG)
			printf("[DEBUG] Client FD = %d\n",cmd->sock_fd);
		#endif
			if ( cmd->sock_fd < 0 ) {
		#if defined(DEBUG)
			printf("[DEBUG] Status code = %d\n",cmd->sock_fd);
		#endif
				error_msg("Could not accept connection");
			}

			status = print_ip_port(clientip,sizeof(clientip),clientport,
			                       sizeof(clientport),&client,client_s);
			if ( status == -1 ) {
		#if defined(DEBUG)
			printf("[DEBUG] Status code = %d\n",status);
		#endif
				error_msg("Could not get address and port of client");
				clean_up(NULL,NULL,&cmd->sock_fd);
			}

			status = welcome_msg(cmd->sock_fd);
			if ( status == -1 ) {
		#if defined(DEBUG)
			printf("[DEBUG] Status code = %d\n",status);
		#endif
				error_msg("Could not send welcome message");
				clean_up(NULL,NULL,&cmd->sock_fd);
			}

			done = 1;
		}
		
		command_type = receive_command(cmd);
		#if defined(DEBUG)
			printf("[DEBUG] Command value = %d\n",command_type);
		#endif
		if ( command_type == ER_CMD ) {
			error_msg("Incorrect command");
			clean_up(NULL,NULL,&cmd->sock_fd);
			done = 0;
		}

		status = do_command(command_type,cmd);
		if ( status < 0 ) {
		#if defined(DEBUG)
			printf("[DEBUG] Status code = %d\n",status);
		#endif
			error_msg("Command Error");
			clean_up(NULL,NULL,&cmd->sock_fd);
			done = 0;
		} else {
			shutdown_msg(clientip,clientport);
			shutdown(cmd->sock_fd,SHUT_RDWR);
			done = 0;
		}
	}

	free(cmd);
	clean_up(&process_id,&listen_fd,NULL);

	return 0;
}
