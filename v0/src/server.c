#if defined(SERVER)
#include "net_utility.h"
#include <pthread.h>
#include <netinet/in.h>
#include <dirent.h>
#define BACKLOG          10       /* Listen connections */

void open_directory(char **file_name) {
	
	DIR *dir_fd;
	struct dirent *directory;

	/* Open directory to read from */
	dir_fd = opendir(file_name[3]);
	if ( dir_fd == NULL ) {
		error_msg("Could not open directory");
	}

	/* Read each file name that is in the directory not including . or .. */
	while ( (directory = readdir(dir_fd)) != NULL ) {
		if ( strcmp(directory->d_name,".") == 0 || strcmp(directory->d_name,"..") == 0) { continue; }
		printf("%s\n",directory->d_name);
	}

	/* Close the open directory descriptor */
	closedir(dir_fd);
} 

void start_server(char **argv) {
	
	/* Socket File Descriptors
	   Error Validation     */
	int sock_fd, status, client_fd;

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
	char host_ip[NI_MAXHOST];
	socklen_t host_ip_len;
	char host_port[NI_MAXSERV];
	socklen_t host_port_len;

	/* Information to send and rec */
	/* Methods_t requests = { "Get OK\n","Put OK\n","List OK\n","See OK\n"}; */

	/* Initial message to be sent to client after connection */
	const char *welcome_message = "--------------------------------\n"
	                              "|  Welcome to RPi FTP Server   |\n"
                                  "|------------------------------|\n"
								  "|  Use the following methods:  |\n"
                                  "|------------------------------|\n"
								  "|  Get a file (download file)  |\n"
								  "|  Put a file (upload file)    |\n"
								  "|  List files in directory     |\n"
								  "|  See file content            |\n"
                                  "--------------------------------\n";
	
	/* Variables need to recieve and response to the client(s) 
	   Buffer size for recieve and response is 1024 bytes */
	ssize_t bytes;
	char recieve[BUFFER];
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

	/* Binds the socket with port and address */
	perform_task("Binding socket to port and address...",NULL);
	status = bind(sock_fd,server->ai_addr,server->ai_addrlen);
	if ( status < 0 ) {
		error_msg("Could not bind socket to address and port");
		clean_up(&process_id,&sock_fd,NULL,&server);
		exit(1);
	}

	pass_msg(NULL,NULL);

	/* Socket so now ready to listen to any incoming requests 
	   Max backlog connections is 10                       */
	perform_task("Setting up socket for listening...",NULL);
	status = listen(sock_fd,BACKLOG);
	if ( status < 0 ) {
		error_msg("Could not set up socket for listening.");
		clean_up(&process_id,&sock_fd,NULL,&server);
		exit(1);
	}

	/* This structure is no longer needed */
	clean_up(NULL,NULL,NULL,&server);
	pass_msg(NULL,NULL);

	/* The socket is now waiting for any incoming connections
	   if accepted a new socket descriptor is created      */
	perform_task("Waiting to accept connections...",NULL);
	client_len = sizeof(client_in);
	client_fd = accept(sock_fd,(struct sockaddr*)&client_in,&client_len);
	if ( client_fd < 0 ) {
		error_msg("Could not accept the connection.");
	}

	/* Getting clients information */
	host_ip_len = sizeof(host_ip);
	host_port_len = sizeof(host_port);
	status = getnameinfo((struct sockaddr*)&client_in,client_len,
	                     host_ip, host_ip_len,
						 host_port, host_port_len,
						 NI_NUMERICHOST | NI_NUMERICSERV);
	if ( status < 0 ) {
		error_msg("Could not get host IPv4 address or Port");
		clean_up(NULL,NULL,&client_fd,NULL);
	}

	/* Once accepted print out IPv4:Port that was accepted on server side */
	printf("Created Client FD: %d\n",client_fd);
	printf("Accepted connection from %s:%s\n",host_ip,host_port);

	bytes = send(client_fd,welcome_message,strlen(welcome_message),0);
	if ( bytes == -1 ) {
		error_msg("Could not send bytes to client");	
		clean_up(NULL,NULL,&client_fd,NULL);
	}

	while(1) {
		
		/* Ensure the message buffers are 0 out before (re)sending data */
		memset(recieve,0,sizeof(recieve));
		memset(response,0,sizeof(response));

		bytes = recv(client_fd,recieve,sizeof(recieve),0);
		if ( bytes == -1 ) {
			error_msg("Could not recieve bytes from client");
			clean_up(NULL,NULL,&client_fd,NULL);
		}

		printf("Request: %s",recieve);

		if ( strncmp(recieve,"Get",3) == 0 ) {
			strncpy(response,"Request: Get\nStatus: OK\nFile: \n",sizeof(response)-1);

			bytes = send(client_fd,response,strnlen(response,BUFFER),0);
			if ( bytes == -1 ) {
				error_msg("Could not send get response to client");
				clean_up(NULL,NULL,&client_fd,NULL);
				exit(1);
			}
			printf("Server response complete...\n");
			printf("Disconnecting from client...\n");
			clean_up(NULL,NULL,&client_fd,NULL);
			break;
		}
		else if ( strncmp(recieve,"List",4) == 0 ) {
			strncpy(response,"Request: List\nStatus: OK\nFiles:\n",sizeof(response)-1);

			DIR *dir_fd;
			struct dirent *directory;

			/* Open directory to read from */
			dir_fd = opendir(argv[3]);
			if ( dir_fd == NULL ) {
				error_msg("Could not open directory");
			}

			/* Read each file name that is in the directory not including . or .. */
			while ( (directory = readdir(dir_fd)) != NULL ) {
				if ( strcmp(directory->d_name,".") == 0 || strcmp(directory->d_name,"..") == 0) { continue; }
				strncat(response,directory->d_name,256);
				strcat(response,"\n");
			}

			/* Close the open directory descriptor */
			closedir(dir_fd);
			bytes = send(client_fd,response,strnlen(response,BUFFER),0);
			if ( bytes == -1 ) {
				error_msg("Could not send get response to client");
				clean_up(NULL,NULL,&client_fd,NULL);
				exit(1);
			}
		}
		else {
			strncpy(response,"Invalid request... Closing connection\n",sizeof(response)-1);

			bytes = send(client_fd,response,strnlen(response,BUFFER),0);
			if ( bytes == -1 ) {
				error_msg("Could not send get response to client");
				clean_up(NULL,NULL,&client_fd,NULL);
				exit(1);
			}
			error_msg("Invalid input disconnecting from client...");
			clean_up(NULL,NULL,&client_fd,NULL);
			exit(1);
		}
	}

	clean_up(&process_id,&sock_fd,NULL,NULL);
}
#endif /* End TEST_SERVER */
