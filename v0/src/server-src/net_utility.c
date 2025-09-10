#include <stdio.h>
#include "net_utility.h"

void format(int count,const char character) {

	for (int i=0 ; i<count ; i++) {
		putc(character,stdout);
	}
	putc('\n',stdout);
}

void usage(char *argv[]) {
	format(40,'-');
	printf("  Usage: %s [IPv4] [Port] [DIR].\n",argv[0]);
	printf("Example: %s 192.168.0.xxx 8000 /home/user.\n",argv[0]);
	printf("   Note: Port has to be 1023 > port < 65535. See /etc/protocols.\n");
	format(40,'-');
}

void error_msg(const char *message) {
	fprintf(stderr,"%s\n",message);
}

void clean_up(pid_t *proc_id,int *sock_fd,int *client_fd) {

	if ( proc_id != NULL ) fprintf(stderr,"Killing process: %d\n",*proc_id);
	if ( sock_fd != NULL ) close(*sock_fd);
	if ( client_fd != NULL ) close(*client_fd);
}

void perform_task(const char *task_msg,int *task_id) {
	(task_id == NULL) ? printf("%s\n",task_msg) : printf("%s %d\n",task_msg,*task_id);
}

void pass_msg(const char *task,int *task_id) {
	if ( task != NULL && task_id != NULL ) printf("%s %d\n",task,*task_id);
	int count = 35;	
	for (int i=0; i < count; i++) {
		putchar('-');
		if ( i == count-1 ) {
			putchar('>');
			puts(" Done");
		}
	} 
}
