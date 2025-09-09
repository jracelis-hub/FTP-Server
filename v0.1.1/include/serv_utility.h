#ifndef SERV_UTILITY_H
#define SERV_UTILITY_H
#include <sys/socket.h>
#include <netinet/in.h>

int welcome_msg(int client_fd);
int print_ip_port(struct sockaddr_in *client,socklen_t client_len);

#endif /* SERV_UTILITY_H */
