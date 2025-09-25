#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>

bool isasemicolon(int c);
bool isaslash(int s);
bool isaspace(int s);
bool isanewline(int n);
bool isdirectoryformat(char *directory);

#endif /* PARSING_H */
