#if defined(SERVER)
#include "net_utility.h"
#include "commands.h"
#include <netinet/in.h>
#include <pthread.h>
#define BACKLOG          10       /* Listen connections */

/* Initial message to be sent to client after connection */
static const char *welcome_message = 
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

void start_server(char *argv[]) {
	
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
	ssize_t bytes;
	char receive[BUFFER];
	char response[BUFFER];

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
		
		int done = 0;

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
			   IP & Port                 */
			status = print_client_ip(&client_in,&client_len);
			if ( status == -1 ) {
				error_msg("Could not get host IPv4 address or Port");
				clean_up(NULL,NULL,&client_fd,NULL);
			}

			/* The initial message sent to the client on how to interface
			   with the FTP server                                     */
			status = server_welcome_msg(&client_fd,&bytes,welcome_message);
			if ( status != 0 ) {
				error_msg("Could not send welcome message");
				clean_up(NULL,NULL,&client_fd,NULL);
			}
			printf("Bytes sent: %zu\n",bytes);
			done = 1;
		}

		/* This wrapper function handles the request from the client 
		   If the request returns one of the following macros it will
		   be sent to do_cmd function                              */
		status = check_cmd(&client_fd,&bytes,receive);
		if ( status == -1 ) {
			error_msg("Invalid command: %s");
			clean_up(NULL,NULL,&client_fd,NULL);
		}

		/* After function returns request do_cmd does the following
		   cmd and sends back the data requested from the client */
		status = do_cmd(status,&client_fd,&bytes,response,argv);
		if ( status == 1 ) {
			error_msg("Could not send bytes");
			clean_up(NULL,NULL,&client_fd,NULL);
		}
	}
	clean_up(&process_id,&listen_fd,NULL,NULL);
}
#endif /* End TEST_SERVER */
