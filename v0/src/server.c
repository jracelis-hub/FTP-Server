#if defined(SERVER)
#include "net_utility.h"
#include "commands.h"
#include "server_utility.h"
#include <netinet/in.h>
#define BACKLOG          10       /* Listen connections */

void start_server(char **argv) {
	
	/* Socket File Descriptors
	   Error Validation     */
	int listen_fd, status, client_fd;

	/* Structure to hold information for the server */
	struct addrinfo hints, *server;

	/* Command line arguments from user
	   IPv4 max character = 15 + '\0' = 16
	   Port max character = 5 + '\0' = 6  */
	char ip[IP_MAX_LEN];
	char port[PORT_MAX_LEN];

	/* Structure to hold client information 
	   ONLY HOLDS IPv4!                   */
	struct sockaddr_in client_in;
	socklen_t client_len;

	/* Variables need to receive and response to the client(s) 
	   Buffer size for receive and response is 1024 bytes */
	char receive[BUFFER];
	char response[BUFFER];
		
	/* Macros needed to hold 
	   IPv4 & Port size   */
	char host_ip[NI_MAXHOST];
	char host_port[NI_MAXSERV];

	int done = 0;

	/* Get process id of the server running */
	pid_t process_id = getpid();
	perform_task("Created new process:",(int *)&process_id);

	/* Copying command line arguments into: 
	   IPv4: 192.168.0.241 or
	   Local: 127.0.0.1
	   Port: 8000                        */
	if ( inet_pton(AF_INET,argv[1],ip) == 1 )
	{
		strcpy(ip,argv[1]);	
		strcpy(port,argv[2]);	
	}
	else {
		error_msg("The IPv4 address argv[1] is not a proper IPv4 format");
		usage(argv);
		clean_up(&process_id,NULL,NULL,NULL);
		exit(1);
	}

	/* Important to zero out any information in the hints structure             
	   Server Type:
	   IPv4
	   TCP                               
	   Using user input for IPv4 Server addr                     */
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;

	/* Resolve server information */
	perform_task("Resolving server information...",NULL);
	status = getaddrinfo(ip,port,&hints,&server);
	if ( status != 0 ) {
		error_msg("Could not resolve server information...");
		clean_up(&process_id,NULL,NULL,NULL);
		exit(1);
	}

	pass_msg(NULL,NULL);
	
	/* Creates a socket descriptor to listen on */
	perform_task("Creating socket...",NULL);
	listen_fd = socket(server->ai_family,server->ai_socktype,server->ai_protocol);
	if ( listen_fd < 0 ) {
		error_msg("Could not create a socket");
		clean_up(&process_id,NULL,NULL,&server);
		exit(1);
	}

	pass_msg("Created Socket FD:",&listen_fd);

	/* Binds the socket with port and address */
	perform_task("Binding socket to port and address...",NULL);
	status = bind(listen_fd,server->ai_addr,server->ai_addrlen);
	if ( status < 0 ) {
		error_msg("Could not bind socket to address and port");
		clean_up(&process_id,&listen_fd,NULL,&server);
		exit(1);
	}

	pass_msg(NULL,NULL);

	/* Socket so now ready to listen to any incoming requests 
	   Max backlog connections is 10                       */
	perform_task("Setting up socket for listening...",NULL);
	status = listen(listen_fd,BACKLOG);
	if ( status < 0 ) {
		error_msg("Could not set up socket for listening.");
		clean_up(&process_id,&listen_fd,NULL,&server);
		exit(1);
	}

	/* This structure is no longer needed */
	clean_up(NULL,NULL,NULL,&server);
	pass_msg(NULL,NULL);

	while(1) {

		/* Ensure the message buffers are 0 out before (re)sending data */
		memset(receive,0,sizeof(receive));
		memset(response,0,sizeof(response));

		if (!done) {
			/* The socket is now waiting for any incoming connections
			   if accepted a new socket descriptor is created      */
			perform_task("Waiting to accept connections...",NULL);
			client_len = sizeof(client_in);
			client_fd = accept(listen_fd,(struct sockaddr*)&client_in,&client_len);
			if ( client_fd < 0 ) {
				error_msg("Could not accept the connection.");
			}

			/* Getting clients information:
			   IP & Port                */
			status = print_client_ip(host_ip,sizeof(host_ip),
			                         host_port,sizeof(host_port),
									 &client_in,&client_len);
			if ( status == ERROR ) {
				error_msg("Could not get host IPv4 address or Port");
				clean_up(NULL,NULL,&client_fd,NULL);
			}

			/* The initial message sent to the client on how to interface
			   with the FTP server                                     */
			status = server_welcome_msg(client_fd);
			if ( status != 0 ) {
				error_msg("Could not send welcome message");
				disconnect_host(host_ip,host_port);
				clean_up(NULL,NULL,&client_fd,NULL);
			}

			done = 1;
		}

		/* This wrapper function handles the request from the client 
		   If the request returns one of the following macros it will
		   be sent to do_cmd function                              */
		status = check_cmd(client_fd,receive,sizeof(receive));
		if ( status == ERROR ) {
			send_exit_cmd(client_fd,receive,response,sizeof(response));
			error_msg("Invalid command");
			disconnect_host(host_ip,host_port);
			clean_up(NULL,NULL,&client_fd,NULL);
			done = 0;
		}

		/* After function returns request do_cmd does the following
		   cmd and sends back the data requested from the client */
		status = do_cmd(status,client_fd,response,sizeof(response),receive,argv);
		if ( status == ERROR ) {
			send_exit_cmd(client_fd,NULL,response,sizeof(response));
			error_msg("Unable to send a response to client");
			disconnect_host(host_ip,host_port);
			clean_up(NULL,NULL,&client_fd,NULL);
			done = 0;
		} else if ( status == ER_OVERFLOW ) {
			send_exit_cmd(client_fd,NULL,response,sizeof(response));
			error_msg("Buffer over flow... size too large");
			disconnect_host(host_ip,host_port);
			clean_up(NULL,NULL,&client_fd,NULL);
			done = 0;
		} else if ( status == ER_BYTES ) {
			send_exit_cmd(client_fd,NULL,response,sizeof(response));
			error_msg("Buffer over flow... size too large");
			disconnect_host(host_ip,host_port);
			clean_up(NULL,NULL,&client_fd,NULL);
			done = 0;
		}
	}
	
	printf("Server shutting down...\n");
	clean_up(&process_id,&listen_fd,NULL,NULL);
}
#endif /* End TEST_SERVER */
