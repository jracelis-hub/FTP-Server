# Compiler Used and Variables
CC=gcc
INCLUDE_DIR=-I../include
S_DEFINE=-DSERVER
C_DEFINE=-DCLIENT
CFLAGS=-Wall -Wextra

SERVER=server
CLIENT=client

C_SRC=client.c net_utility.c

S_SRC=server.c net_utility.c

S_OBJ=s_main.o server.o net_utility.o
S_OBJ_N=s_main.o
C_OBJ=c_main.o client.o net_utility.o
C_OBJ_N=c_main.o

.PHONY: clean-all clean-client clean-server

all: $(SERVER) $(CLIENT)

# To Build Server 

$(SERVER): $(S_OBJ)
	$(CC) -o $@ $^

$(S_OBJ): $(S_SRC)
	$(CC) $(FLAGS) $(S_DEFINE) -c $^ $(INCLUDE_DIR)

$(S_OBJ_N): main.c
	$(CC) $(FLAGS) $(S_DEFINE) -c $< -o $@ $(INCLUDE_DIR)

# To Build Client 

$(CLIENT): $(C_OBJ)
	$(CC) -o $@ $^

$(C_OBJ): $(C_SRC)
	$(CC) $(FLAGS) $(C_DEFINE) -c $^ $(INCLUDE_DIR) 

$(C_OBJ_N): main.c
	$(CC) $(FLAGS) $(C_DEFINE) -c $< -o $@ $(INCLUDE_DIR) 

# To Clean Server
clean-server:
	rm -rf $(S_OBJ) $(SERVER)

clean-client:
	rm -rf $(C_OBJ) $(CLIENT)
