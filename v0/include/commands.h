#ifndef COMMANDS_H
#define COMMANDS_H
#include <stddef.h>

#define DOWNLOAD 0
#define UPLOAD 1
#define LIST 2
#define READ 3
#define BUFFER 4096

typedef struct {
	char reply[BUFFER];
	char request[BUFFER];
	char directory[128];
	size_t reply_s;
	size_t request_s;
	int sock_fd;
} cmd_t;

int receive_command(cmd_t *cmd);
int do_command(int command,cmd_t *cmd);
int download_command(cmd_t *cmd);
int upload_command(cmd_t *cmd);
int list_command(cmd_t *cmd);
int read_command(cmd_t *cmd);

#endif /* COMMANDS_H */
