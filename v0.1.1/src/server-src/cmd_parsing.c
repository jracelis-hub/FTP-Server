#include "cmd_parsing.h"
#include <stddef.h>

char *parse_request(char *request) {
	
	int i;
	char *temp = NULL;

	for (i=0; *(request+i) != '\0'; i++) {
		if ( *(request+i) == ' ' ) {
			temp = (request+i);
		}
		if ( *(request+i) == ';' ) {
			break;
		}
		if ( *(request+i) == '\n' ) {
			*(request+i) = '\0';
			break;
		}
	}

	if (temp)
		return temp + 1;
	else 
		return NULL;
}

char *parse_file(char *request) {

	int i;
	char *temp = NULL;

	for (i=0; *(request+i) != '\0'; i++) {
		if ( *(request+i) == ';' || 
		     *(request+i) == ' ' || 
			 *(request+i) == '/' ) 
		{
			temp = (request+i);
		}
		if ( *(request+i) == '\n' ) {
			*(request+i) = '\0';
			break;
		}
	}
	
	if (temp)
		return temp + 1;
	else
		return NULL;
}
