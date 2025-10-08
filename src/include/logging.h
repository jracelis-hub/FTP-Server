#ifndef LOGGING_H
#define LOGGING_H

#define SUCCESS 0

/* Error types */
#define ERROR_OVERFLOW (-2)
#define ERROR_ALLOC (-3)
#define ERROR_FILE (-4)
#define ERROR_DIR (-5)
#define ERROR_THREADS (-6)
#define ERROR_SHUTDOWN (-7)
#define ERROR_NAMEINFO (-8)
#define ERROR_COMMAND (-9)
#define ERROR_BYTES (-10)
#define ERROR_NULL (-11)
#define ERROR_SERVER (-20)
#define ERROR_SOCKET_OPT (-30)

/* prints usage message from invalid arguments passed in */
void usage(char *program);

/* logging for error message and performing a task */
void error_msg(const char *message);
void perform_msg(const char *message);
void pass_msg(void);

/* Used just for creating a line of characters */
void format(int count, const char character);

#endif /* LOGGING_H */
