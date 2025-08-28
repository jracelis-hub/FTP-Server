# -------- Compiler Settings ------- #
CC=gcc
CLIENT_D=-DCLIENT
SERVER_D=-DSERVER
INCDIR=-I../include
CFLAGS=-Wall -Wextra

# -------- Executable Names -------- #
SERVER=server
CLIENT=client

# ---------- Object Files ---------- #
# S = Server
# C = Client
OBJ_S=main.o net_utility.o server.o
OBJ_C=main.o net_utility.o client.o

# ---------- Source Files ---------- #
# S = Server
# C = Client
SRC_S=main.c net_utility.c server.c
SRC_c=main.c net_utility.c client.c

.Phony: clean,clean-server,clean-client

# ---------- Make ----------
all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJ_S)
	$(CC) -o $@ $^

$(CLIENT): $(OBJ_C)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) $(SERVER_D) $(INCDIR) -c -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) $(CLIENT_D) $(INCDIR) -c -o $@ $^

clean:
	rm -rf $(SERVER) $(OBJ_S) $(CLIENT) $(OBJ_C)

clean-server:
	rm -rf $(SERVER) $(OBJ_S)

clean-client:
	rm -rf $(CLIENT) $(OBJ_C)
