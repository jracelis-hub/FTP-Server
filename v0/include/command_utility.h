#ifndef COMMAND_UTILITY_H
#define COMMAND_UTILITY_H
#include <stddef.h>

void parse_cmd(int cmd_status,char *file,size_t file_s,char *receive,char **dir_name);
void remove_spaces(char *name,size_t name_len);

#endif /* COMMAND_UTILITY_H */
