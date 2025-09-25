#include <stdio.h>
#include "logging.h"

void format(int count,const char character) 
{
	for (int i=0 ; i<count ; i++) 
		putc(character,stdout);
	putc('\n',stdout);
}

void usage(char *program) 
{
	format(40,'-');
	printf("  Usage: %s [IPv4] [Port] [DIR].\n", program);
	printf("Example: %s 192.168.0.xxx 8000 /home/user.\n", program);
	printf("   Note: Port has to be 1023 > port < 65535. See /etc/protocols.\n");
	format(40,'-');
}

void error_msg(const char *message)
{
	fprintf(stderr,"%s\n",message);
}

void perform_msg(const char *message)
{
	printf("%s\n", message);
}

void pass(void) 
{
	int count = 35;	
	for (int i = 0; i < count; i++) 
	{
		putchar('-');
		if ( i == count - 1 ) 
		{
			putchar('>');
			puts(" Done");
		}
	} 
}
