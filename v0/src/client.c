#if defined(CLIENT)
#include "net_utility.h"
int check_for_exit(char *receive) {

	char *found;
	const char *token = "\r\n";

	found = strstr(receive,token);

	if (found != NULL) {
		return ERROR;		
	}
	return 0;
}

void start_client(char **argv) {
	
	/* Socket File Descriptors
	   Error Validation     */
	int sock_fd, status;

	/* Structure to resolving the server information */
	struct addrinfo hints, *server;

	/* Command line arguments from user
	   IPv4 max character = 15 + '\0' = 16
	   Port max character = 5 + '\0' = 6  */
	char ip[IP_MAX_LEN];
	char port[PORT_MAX_LEN];

	/* Bytes to receive or send of a response 
	   Buffer size of 1024 characters */
	ssize_t bytes;
	char response[BUFFER];
	char receive[BUFFER];

	/* Gives the process id of the FTP server */
	pid_t process_id = getpid();
	perform_task("Create new process:",(int *)&process_id);

	if ( inet_pton(AF_INET,argv[1],ip) == 1 )
	{
		strcpy(ip,argv[1]);	
		strcpy(port,argv[2]);	
	}
	else {
		error_msg("The given arguments are out of max character range");
		clean_up(&process_id,NULL,NULL,NULL);
		usage(argv);
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
	if ( status < 0 ) {
		error_msg("Could not resolve server information...");
		clean_up(&process_id,NULL,NULL,NULL);
		exit(1);
	}

	pass_msg(NULL,NULL);
	
	/* Creates a socket descriptor to listen on */
	perform_task("Creating socket...",NULL);
	sock_fd = socket(server->ai_family,server->ai_socktype,server->ai_protocol);
	if ( sock_fd < 0 ) {
		error_msg("Could not create a socket");
		clean_up(&process_id,NULL,NULL,&server);
		exit(1);
	}

	pass_msg("Created Socket FD:",&sock_fd);
	
	perform_task("Attempting to connect to server...",NULL);
	status = connect(sock_fd,server->ai_addr,server->ai_addrlen);
	if ( status < 0 ) {
		error_msg("Failed to connect server.");
		clean_up(&process_id,&sock_fd,NULL,&server);
		exit(1);
	}

	/* Done with linked list */
	clean_up(NULL,NULL,NULL,&server);

	printf("Connected to %s:%s\n",argv[1],argv[2]);

	/* Begin Data transfering */
	while(1) {

		/* Zero out the buffers to avoid receive old data */
		memset(receive,0,sizeof(receive));
		memset(response,0,sizeof(response));

		bytes = recv(sock_fd,receive,sizeof(receive),0);
		if ( bytes == -1 ) {
			error_msg("Could not receive bytes from server");
			clean_up(&process_id,&sock_fd,NULL,NULL);
			exit(1);
		}

		printf("Received bytes: %zu\n",bytes);
		printf("%s",receive);

		status = check_for_exit(receive);
		if ( status == ERROR ) {
			break;	
		}

		printf("$ ");
		fgets(response,BUFFER-1,stdin);
		bytes = send(sock_fd,response,strnlen(response,BUFFER),0);
		if ( bytes == -1 ) {
			error_msg("Could not receive bytes from server");
			clean_up(&process_id,&sock_fd,NULL,NULL);
			exit(1);
		}
	}

	perform_task("Process killed:",(int *)&process_id);
	clean_up(&process_id,&sock_fd,NULL,NULL);
}
#endif /* End TEST_CLIENT */
