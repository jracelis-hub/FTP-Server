# Compiler
CC=gcc
WFLAGS=-Wall -Werror
DEBUG=-g
INCLUDE=-I../../include
CFLAGS=$(WFLAGS) $(DEBUG) $(INCLUDE)

SRC=server.c net_utility.c serv_utility.c commands.c cmd_parsing.c
OBJ=server.o net_utility.o serv_utility.o commands.o cmd_parsing.o
SERVER=server

IP?=127.0.0.1
PORT?=8000
DIR?=$(HOME)

.PHONY: run-server clean

run-server: $(SERVER)
	@clear
	./$(SERVER) $(IP) $(PORT) $(DIR)

$(SERVER): $(OBJ)
	$(CC) -o $@ $^ 

%.c:%.o
	$(CC) $(CFLAGS) -c $@ -o $^

clean: $(SERVER) $(OBJ)
	rm -r $^
