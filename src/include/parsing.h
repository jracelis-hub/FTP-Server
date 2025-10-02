#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>
#include <stddef.h>

bool isasemicolon(int c);
bool isaslash(int s);
bool isaspace(int s);
bool isanewline(int n);
bool isdirectoryformat(char *directory);
char *parse_request_get_payload(char *payload);
char *parse_request_get_command(char *command);
int parse_request_get_file(char *request, char *file, size_t file_size);

#endif /* PARSING_H */
