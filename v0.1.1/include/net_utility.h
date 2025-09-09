#ifndef NET_UTILITY_H
#define NET_UTILITY_H
#include <unistd.h>

#define BUFFER 4056
#define ERROR -1
#define ER_BYTES -2
#define ER_CMD -3
#define ER_FILE -4

void format(int count,const char character);
void usage(char *argv[]);
void error_msg(const char *message);
void clean_up(pid_t *proc_id,int *sock_fd,int *client_fd);
void perform_task(const char *task_msg,int *task_id);
void pass_msg(const char *task,int *task_id);

#endif /* NET_UTILITY_H */
