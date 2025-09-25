#ifndef CLIENT_COMMANDS_H
#define CLIENT_COMMANDS_H

#include "command_type.h"

command_t command_return(char *request);
void command_get_file(char *request, char *file, int size);
int command_handle_upload(char *reply, size_t reply_size);

#endif /* CLIENT_COMMANDS_H */
