#ifndef COMMANDS_H
#define COMMANDS_H

#include <stddef.h>

#define BUFFER 1024

typedef struct thread_handler_t thread_handler_t;

void command_receive_from_client(thread_handler_t *thread_handle);
int command_handler(thread_handler_t *thread_handle);

int command_handle_list(thread_handler_t *thread_handle);
int command_handle_upload(char *request, char *reply, size_t reply_size, char *directory);
int command_handle_read(thread_handler_t *thread_handle);

void command_handle_invalid(thread_handler_t *thread_handle);

#endif /* COMMANDS_H */
