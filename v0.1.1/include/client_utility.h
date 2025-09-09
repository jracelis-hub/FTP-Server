#ifndef CLIENT_UTILITY_H
#define CLIENT_UTILITY_H
#include <stddef.h>

int get_request(int sock_fd,char *request,size_t request_s);
int send_request(int sock_fd,char *reply,size_t reply_s);
int check_for_exit(char *request);

#endif /* CLIENT_UTILITY_H */
