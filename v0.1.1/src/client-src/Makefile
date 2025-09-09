# Compiler
CC=gcc
WFLAGS=-Wall -Werror
DEBUG=-g
INCLUDE=-I../../include
CFLAGS=$(WFLAGS) $(DEBUG) $(INCLUDE)

SRC=client.c net_utility.c client_utility.c
OBJ=client.o net_utility.o client_utility.o
CLIENT=client

IP?=127.0.0.1
PORT?=8000
DIR?=$(HOME)

.PHONY: run-client clean

run-client: $(CLIENT)
	@clear
	./$(CLIENT) $(IP) $(PORT) $(DIR)

$(CLIENT): $(OBJ)
	$(CC) -o $@ $^ 

%.c:%.o
	$(CC) $(CFLAGS) -c $@ -o $^

clean: $(CLIENT) $(OBJ)
	rm -r $^
