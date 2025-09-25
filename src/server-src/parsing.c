#include "parsing.h"
#include <string.h>

bool isasemicolon(int c)
{
	bool semicolon = true;

	if ((char)c == ';')
		return semicolon;
	
	semicolon = false;
	return semicolon;
}

bool isaslash(int s)
{
	bool slash = true;

	if ((char)s == '/')
		return slash;
	
	slash = false;
	return slash;
}

bool isaspace(int s)
{
	bool space = true;

	if ((char)s == ' ')
		return space;

	space = false;
	return space;
}

bool isanewline(int n)
{
	bool newline = true;

	if ((char)n == '\n')
		return newline;
	
	newline = false;
	return newline;
}

bool isdirectoryformat(char *directory)
{
	bool dir = true;

	size_t dir_len = strlen(directory);

	if ( *(directory + dir_len - 1) == '/')
		return dir;
	
	dir = false;
	return dir;
}
