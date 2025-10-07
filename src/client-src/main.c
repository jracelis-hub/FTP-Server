#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>

#include "logging.h"
#include "client.h"
#include "client-commands.h"

int main(int argc, char *argv[]) {
	
	/* Requrires ./program IP Port arguments */
	if (argc != 3) 
	{
		usage(argv[0]);
		return -1;
	}

	int port = atoi(argv[2]);

	if (port < 1024 || port > 65535) 
	{
		error_msg("Port has to be above 1023 or below 65535");
		usage(argv[0]);
		return -1;
	}

	int socket_fd;
	struct sockaddr_storage server = {0};

	perform_msg("Creating socket...");
	#if defined(INET) 
		if (client_create_stream_socket_ip4(&socket_fd) < 0)
		{
			error_msg("Could not create socket");
			return -1;
		}
	#elif defined(INET6) 
		if (client_create_stream_socket_ip6(&socket_fd) < 0)
		{
			error_msg("Could not create socket");
			return -1;
		}
	#endif

	pass_msg();

	perform_msg("Attempting to connect to server...");
	#if defined(INET) 
		if (client_connect_to_server_ip4(socket_fd, &server, port, argv[1]) < 0)
		{
			error_msg("Could not connect to server");
			close(socket_fd);
			return -1;
		}
	#elif defined(INET6)
		if (client_connect_to_server_ip6(socket_fd, &server, port, argv[1]) < 0)
		{
			error_msg("Could not connect to server");
			close(socket_fd);
			return -1;
		}
	#endif
		
	pass_msg();

	if (client_print_connection_message(&server) < 0) 
	{
		error_msg("Could not resolve server information");
		return -2;
	}

	/* Creates a client handler to be used for communication
	 * between server <-> client                         */
	client_handler_t client_handle;
	client_create_client_handler(&client_handle, socket_fd);

	while (1)
	{
		client_set_buffers_zero(&client_handle);

		client_received_bytes_from_server(&client_handle);
		if (client_handle.received_bytes < 0) 
		{
			error_msg("Could not receive bytes from server");
			close(socket_fd);
			return -1;
		}

		client_send_request_to_server(&client_handle);

		client_cache_request(&client_handle);

		client_send_bytes_to_server(&client_handle);
		if (client_handle.send_bytes < 0) 
		{
			error_msg("Could not send bytes to server");
			close(socket_fd);
			return -1;
		}
	}

	close(socket_fd);
	return 0;
}
