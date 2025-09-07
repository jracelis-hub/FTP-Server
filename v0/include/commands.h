#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <stddef.h>

/* ---------- Receives input from client to see if its in proper format ---------- */
int check_cmd(int client_fd,char *receive,size_t receive_buffer);

/* ---------- Return status from check_cmd for do cmd to preform ---------- */
#define DOWNLOAD         12
#define UPLOAD           13
#define LIST             14
#define READ             15


/* ---------- Grabs input from check cmd to do the follow command below ---------- */
int do_cmd(int cmd_status,int client_fd,char *response,size_t response_buffer,char *receive,char **dir_name);

/* ---------- Download Command ------------ */

/* ---------- Upload Command ------------ */

/* ---------- List Command ------------ */
int list_cmd(int client_fd,char *request,char *response,size_t response_buffer,char **dir_name);

/* ---------- Read Command ------------ */
int rod_cmd(int client_fd,char *file,char *request,char *response,size_t response_len,char **dir_name);

/* ---------- Exit Command ------------ */
void send_exit_cmd(int client_fd,char *receive,char *response,size_t response_buffer);

#endif /* _COMMANDS_H */
