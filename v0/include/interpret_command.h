#ifndef INTERPRET_COMMAND_H
#define INTERPRET_COMMAND_H
#include <stddef.h>
#include <sys/types.h>

int interpret_cmd(int sock_fd,char *receive,size_t receive_s);
int interpret_download(char *receive,ssize_t bytes,char *file);
void parse_response(char *file,size_t file_s,char *request,size_t request_s,char *receive);

#endif /* INTERPRET_COMMAND_H */
