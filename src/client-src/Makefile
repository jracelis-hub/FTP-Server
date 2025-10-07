# Compiler
CC=gcc
WFLAGS=-Wall -Werror
DEBUG=-g
INCLUDE=-I../include
CFLAGS=$(WFLAGS) $(DEBUG) $(INCLUDE)

BUILD_DIR=./build
OBJ_DIR=./obj
SRC:=main.c client.c logging.c commands-client.c parsing.c
OBJ:=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
CLIENT:=$(addprefix $(BUILD_DIR)/,client)

IP?=127.0.0.1
PORT?=8000
ifeq ($(INET),1)
	CFLAGS+= -DINET
endif
ifeq ($(INET6),1)
	CFLAGS+= -DINET6
endif

.PHONY: run-client clean make-dir

all: $(CLIENT)

run-client: $(CLIENT)
	@clear
	$(CLIENT) $(IP) $(PORT)

$(CLIENT): $(OBJ) | make-dir
	$(CC) -o $@ $^ 

$(OBJ_DIR)/%.o:%.c | make-dir
	$(CC) $(CFLAGS) -c $< -o $@

make-dir:
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR)

clean: 
	rm -rf $(BUILD_DIR) $(OBJ_DIR)
