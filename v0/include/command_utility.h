#ifndef COMMAND_UTILITY_H
#define COMMAND_UTILITY_H
#include <stddef.h>

void parse_cmd(char *file,size_t file_s,char *request,size_t request_s,char *receive);

void remove_spaces(char *name,size_t name_len);

#endif /* COMMAND_UTILITY_H */
