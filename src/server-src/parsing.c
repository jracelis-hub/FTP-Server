#include "parsing.h"
#include "logging.h"
#include <string.h>
#include <stdio.h>

bool isasemicolon(int c)
{
	bool semicolon = true;

	if ((char)c == ';')
	{
		return semicolon;
	}
	
	semicolon = false;
	return semicolon;
}

bool isaslash(int s)
{
	bool slash = true;

	if ((char)s == '/')
	{
		return slash;
	}
	
	slash = false;
	return slash;
}

bool isaspace(int s)
{
	bool space = true;

	if ((char)s == ' ')
	{
		return space;
	}

	space = false;
	return space;
}

bool isanewline(int n)
{
	bool newline = true;

	if ((char)n == '\n')
	{
		return newline;
	}
	
	newline = false;
	return newline;
}

int parse_request_get_file(char *request, char *file, size_t file_size)
{
	int i = 0;
	char *p;
	for (p = request; *p != '\0' && *p != '\n'; p++) 
	{
		/* The following looks for a ; / or a ' ' if the following is met
		 * the pointer to request will be set after it 
		 * Example: Download; /path/file.txt\n
		 * once Download; request is now pointing to _/path/file.txt
		 * until is is left with file.txt                          */
		if (isasemicolon(*p) || isaslash(*p) || isaspace(*p)) {
			request = p;
			request++;
		}
	}

	/* Starting from the pointed location set from the for loop above 
	 * each character is placed into the array that was passed into         
	 * file[0] = f
	 * file[1] = i
	 * file[2] = l 
	 * and so on until the newline is met */
	while (*request != '\0' && *request != '\n' && i != file_size - 1) 
	{
		file[i++] = *request++;
	}
	file[i] = '\0';

	if (i == file_size - 1) 
	{ 
		return ERROR_OVERFLOW; 
	}

	return SUCCESS;
}

char *parse_request_get_command(char *command)
{
	char *p;
	for (p = command; *p != '\0'; p++)
	{
		/* Looks for the semi-colon once found 
		 * increments past the semi-colon and
		 * null teriminate the string and return it */
		if (isasemicolon(*p)) 
		{
			*(p + 1) = '\0';
			return command;
		}
		/* For the request does not return a proper format
		 * the next item it looks for is a newline/linefeed
		 * if found it returns the invalid request sent  */
		else if (isanewline(*p)) 
		{
			*p = '\0';
			return command;
		}
	}
	return NULL;
}

char *parse_request_get_payload(char *payload)
{
	char *p = payload;
	while (*p != '\0')
	{
		if (isanewline(*p))
		{
			payload = p;
			break;
		}
		p++;
	}

	if (!payload) 
	{
		return payload;
	}
	return ++payload;
}
