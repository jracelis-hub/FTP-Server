#ifndef SERV_UTILITY_H
#define SERV_UTILITY_H
#include <sys/socket.h>
#include <netinet/in.h>

int welcome_msg(int client_fd);
int print_ip_port(char *clientip,size_t clientips,
                  char *clientport,size_t clientports,
				  struct sockaddr_storage *client,socklen_t client_s);
void shutdown_msg(char *clientip,char *clientport);

#endif /* SERV_UTILITY_H */
