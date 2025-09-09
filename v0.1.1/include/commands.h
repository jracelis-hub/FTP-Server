#ifndef COMMANDS_H
#define COMMANDS_H
#include <stddef.h>

#define DOWNLOAD 10
#define UPLOAD 11
#define LIST 12
#define READ 13

int get_command(int client_fd,char *request,size_t request_s);
int list_cmd(int client_fd,char *reply,size_t reply_s,char *dir);
void send_close_connection(int client_fd,char *response,char *reply,size_t reply_s);

#endif /* COMMANDS_H */
