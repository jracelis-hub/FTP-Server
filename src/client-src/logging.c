#include <stdio.h>
#include "logging.h"

void format(int count,const char character) 
{
	int i;
	for (i = 0; i < count; i++) 
	{
		putc(character,stdout);
	}
	putc('\n',stdout);
}

void usage(char *program) 
{
	format(40,'-');
	printf("  Usage: %s [IPv4] [Port].\n",program);
	printf("Example: %s 192.168.0.xxx 8000.\n",program);
	printf("   Note: Port has to be 1023 > port < 65535. See /etc/protocols.\n");
	format(40,'-');
}

void perform_msg(const char *message)
{
	fprintf(stderr, "       File: %s\n", __FILE__);
	fprintf(stderr, "   Function: %s\n", __func__);
	fprintf(stderr, "       Line: %d\n", __LINE__);
	fprintf(stderr, "Description: %s\n", message);
}

void error_msg(const char *message) 
{
	fprintf(stderr, "%s\n",message);
}

void pass_msg(void) {
	int count = 35;	
	int i;
	for (i = 0; i < count; i++) 
	{
		putchar('-');
		if ( i == count-1 ) 
		{
			putchar('>');
			puts(" Done");
		}
	} 
}
