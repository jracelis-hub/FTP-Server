#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>

bool isasemicolon(int c);
bool isaslash(int s);
bool isaspace(int s);
bool isanewline(int n);
bool isdirectoryformat(char *directory);
char *parse_request_get_payload(char *payload);
char *parse_request_get_command(char *command);
void parse_request_get_file(char *request, char *file, int file_size);

#endif /* PARSING_H */
