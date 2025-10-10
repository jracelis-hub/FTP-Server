# Compiler
CC=gcc
WFLAGS=-Wall -Werror
INCLUDE=-I../include
CFLAGS=$(WFLAGS) $(INCLUDE)

# Optional flags 
DFLAGS=-DDEBUG
INET=-DINET
INET6=-DINET6

OBJ_DIR=./obj
BUILD_DIR=./build
SRC:=main.c logging.c server.c thread.c commands-thread.c parsing.c
OBJ:=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
SERVER:=$(addprefix $(BUILD_DIR)/,server)

DEBUG?=0
IP?=127.0.0.1
PORT?=2121
DIR?=$(HOME)
ifeq ($(INET),1)
	CFLAGS+= -DINET
endif
ifeq ($(INET6),1)
	CFLAGS+= -DINET6
endif

.PHONY: run-server build-server clean make-dir all

all: $(SERVER)

run-server: $(SERVER)
	@clear
	$(SERVER) $(IP) $(PORT) $(DIR)

build-server: $(SERVER)

$(SERVER): $(OBJ) | make-dir
	$(CC) -o $@ $^ 

$(OBJ_DIR)/%.o:%.c | make-dir
	$(CC) $(CFLAGS) -c $< -o $@

make-dir:
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR)

clean: 
	rm -r $(OBJ_DIR) $(BUILD_DIR)
