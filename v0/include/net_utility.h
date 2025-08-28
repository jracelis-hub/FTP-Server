#ifndef NET_UTILITY_H  
#define NET_UTILITY_H  

/* Standard C libraries for 
   I/O 
   Strings
   Memory Allocation
   Closing File Descriptors        
   Threads                    */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* Network API libraries */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Macros */
#define BUFFER         1024
#define IP_MAX_LEN       16	      /* xxx.xxx.xxx.xxx + \0 */
#define PORT_MAX_LEN      6	      /* xxxxx + \0 */
#define IP_MIN_LEN        8       /* 10.0.0.0 */
#define REQUEST_TYPE      4
#define STATUS_TYPE       3

/* Functions Prototypes */
void format(int count,const char character);
void usage(char **argv);
void perform_task(const char *task_msg,int *task_id);
void clean_up(pid_t *proc_id,int *sock_fd,int *client_fd,struct addrinfo **server ); /* If the follow does not need freeing put value as NULL */
void error_msg(const char *message);
void pass_msg(const char *task,int *task_id);

/* Server Methods */
typedef struct {
	char methods[REQUEST_TYPE];
		/* const char *download;  To download file from server to local */
		/* const char *upload;	To upload file from local to server */
		/* const char *list; To list files on the server directory */
	int mode;
} command_t;

#if defined(SERVER)
	void open_directory(char **file_name);
	void start_server(char **argv);
#endif

#if defined(CLIENT)
	void start_client(char **argv);
#endif

#endif /* NET_UTILITY_H */
