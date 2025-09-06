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

void parse_cmd(int cmd_status,char *file,size_t file_s,char *receive,char **dir_name) {
	
	const int request_buffer = 16;
	char request[request_buffer];

	/* Grabs input from client and parses it into 
	   request: 
	   file:                                   */
	sscanf(receive,"%[A-Za-z];%[^;\n]",request,file);
	remove_spaces(file,file_s);
}
