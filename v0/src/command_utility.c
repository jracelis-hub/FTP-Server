#include "command_utility.h"
#include <stdio.h>

void remove_spaces(char *name,size_t name_len) {
	
	int i;
	int s = 0;
	for ( i = 0; i<name_len; i++ ) {
		if ( name[i] != ' ' ) {
			name[s++] = name[i];
		}
	}
	name[s] = '\0';
}

void parse_cmd(char *file,size_t file_s,char *request,size_t request_s,char *receive) {
	
	/* Grabs input from client and parses it into 
	   request: 
	   file:                                   */
	sscanf(receive,"%[A-Za-z ];%[^;\n]",request,file);
	remove_spaces(request,request_s);
	remove_spaces(file,file_s);
}
