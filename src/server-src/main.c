/* Standard Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Socket Headers */
#include <netdb.h>

/* User Define Headers */
#include "server.h"
#include "logging.h"
#include "thread.h"

int main(int argc, char *argv[]) 
{
	/* Requres 3 arguments */
	if ( argc != 4 ) 
	{
		usage(argv[0]);
		return -1;
	}

	int port = atoi(argv[2]);

	if ( port < 1024 || port > 65535) 
	{
		error_msg("Port not within boundary");
		usage(argv[0]);
		return -1;
	}

	/* Holds the mounted directory passed in argv[3] 
	 * and appends / if argv[3] does not include it */
	char directory[DIRECTORY_SIZE] = {0};
	if (server_set_directory(directory, DIRECTORY_SIZE, argv[3]) < 0) 
	{
		error_msg("Over flow on directory buffer");
		return -1;
	}

/* Setting up socket structure to hold IPv4 or IPv6
 * and port                                      */
#if defined(INET)
	struct sockaddr_in server_in4 = {0};
	if (server_set_ip4(&server_in4, port, argv[1]) < 0) 
	{
		error_msg("Could not set up ip4");
		return -1;	
	}
#elif defined(INET6)
	struct sockaddr_in6 server_in6 = {0};
	if (server_set_ip6(&server_in6, port, argv[1]) < 0) 
	{
		error_msg("Could not set up ip6");
		return -1;	
	}
#endif

	int listen_fd = 0;

	perform_msg("Creating listening socket...");
#if defined(INET)
	if (server_create_stream_socket_ip4(&listen_fd) < 0) 
	{
		error_msg("Could not create socket");
		return -1;
	}
#elif defined(INET6)
	if (server_create_stream_socket_ip6(&listen_fd) < 0) 
	{
		error_msg("Could not create socket");
		return -1;
	}
#endif

	if (server_set_socket_options(&listen_fd) < 0) 
	{
		error_msg("Could not set reuse address and port information");	
		return -1;
	}

	pass_msg();

	perform_msg("Binding socket to port and address...");
#if defined(INET)
	if (server_bind_socket_ip4(listen_fd, &server_in4) < 0) 
	{
		error_msg("Could not bind socket with port and address");
		close(listen_fd);
		return -1;
	}
#elif defined(INET6)
	if (server_bind_socket_ip6(listen_fd, &server_in6) < 0) 
	{
		error_msg("Could not bind socket with port and address");
		close(listen_fd);
		return -1;
	}
#endif

	pass_msg();

	perform_msg("Listening for incoming connections...");
	if (server_listen_on_socket(listen_fd) < 0) 
	{
		error_msg("Could not listen on socket");
		close(listen_fd);
		return -1;
	}

	while (1) 
	{
		perform_msg("Ready to accept connections...");

		int client_fd;
		struct sockaddr_storage client_address = {0};
		server_accept_client(listen_fd, &client_fd, &client_address);
		if (client_fd < 0) 
		{
			error_msg("Could not accept connection");
			continue;
		}
		
		if (server_create_thread(client_fd, directory, &client_address, &thread_handle_client) < 0)
		{
			error_msg("Could not create thread");
			close(client_fd);
			continue;
		}
	}

	close(listen_fd);
	return 0;
}
