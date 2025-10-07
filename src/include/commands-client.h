#ifndef COMMANDS_CLIENT_H
#define COMMANDS_CLIENT_H

typedef struct client_handler_t client_handler_t;

void command_handler(client_handler_t *client_handle);
void command_handle_empty(client_handler_t *client_handle);
void command_handle_list(client_handler_t *client_handle);
void command_handle_read(client_handler_t *client_handle);
int command_handle_upload(client_handler_t *client_handle);

#endif /* COMMANDS_CLIENT_H */
