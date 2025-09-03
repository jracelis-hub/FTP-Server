#ifndef _COMMANDS_H
#define _COMMANDS_H
#define DOWNLOAD         12
#define UPLOAD           13
#define LIST             14
#define READ             15
#include <sys/socket.h>
#include <netdb.h>

int server_welcome_msg(int *client_fd,ssize_t *bytes,const char *welcome_msg);
int print_client_ip(struct sockaddr_in *client_in,socklen_t *client_len);
int check_cmd(int *client_fd,ssize_t *bytes,char *receive);
int do_cmd(int status,int *client_fd,ssize_t *bytes,char *response,char **dir_name);
void list_cmd(char *response,char **dir_name);

#endif /* _COMMANDS_H */
