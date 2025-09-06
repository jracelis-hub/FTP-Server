#ifndef _SERVER_UTILITY_H
#define _SERVER_UTILITY_H
#include <sys/socket.h>
#include <netinet/in.h>

/* -------------- Initial message to send to client -------------- */
int server_welcome_msg(int client_fd);

/* --------- Prints the connection established from IP:Port --------- */
int print_client_ip(char *host_ip,socklen_t host_ip_len,char *host_port,socklen_t host_port_len, struct sockaddr_in *client_in,socklen_t *client_len);

/* --------- Prints the disconnection from IP:Port --------- */
void disconnect_host(char *host_ip,char *host_port);

#endif /* _SERVER_UTILITY_H */
