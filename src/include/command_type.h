#ifndef COMMAND_TYPE_H
#define COMMAND_TYPE_H

typedef enum command_t { 
	DOWNLOAD,
	UPLOAD,
	LIST,
	READ,
	INVALID = -1,
	EMPTY = 10
} command_t;

#endif /* COMMAND_TYPE_H */
