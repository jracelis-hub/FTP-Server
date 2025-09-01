# ------------ Directory ----------- #
SRC_DIR=./src/
INC_DIR=./include/
BUILD_DIR=./build
C_OBJ_DIR=$(BUILD_DIR)/c_obj
S_OBJ_DIR=$(BUILD_DIR)/s_obj

# ---------- Source Files ---------- #
# S = Server
# C = Client
SRC_S=main.c net_utility.c server.c
SRC_C=main.c net_utility.c client.c

SRC_SD:=$(addprefix $(SRC_DIR),$(SRC_S))
SRC_CD:=$(addprefix $(SRC_DIR),$(SRC_C))

# ------------- Object Files ------------- #
# S = Server
# C = Client
OBJ_S:=$(patsubst $(SRC_DIR)%.c,$(S_OBJ_DIR)/%.o,$(SRC_SD))
OBJ_C:=$(patsubst $(SRC_DIR)%.c,$(C_OBJ_DIR)/%.o,$(SRC_CD))

# ----------- Executable Names ----------- #
SERVER:=$(addprefix $(BUILD_DIR)/,start-server)
CLIENT:=$(addprefix $(BUILD_DIR)/,start-client)

# ----------- Compiler Settings ---------- #
CC=gcc
CLIENT_D=-DCLIENT
SERVER_D=-DSERVER
INC_FLAG=-I$(INC_DIR)
CFLAGS=-Wall -Wextra

# ----------- Running Variables ----------- #
LOCAL?=127.0.0.1
PORT?=2121
MNT_DIR?=$(HOME)

# ----------- Phony Variables ----------- #
.PHONY: all server client \
		run-server run-client \
		clean clean-server clean-client

# ---------- Make Builds ---------- #
all: $(SERVER) $(CLIENT)

run-server: $(SERVER)
	clear
	$(SERVER) $(LOCAL) $(PORT) $(MNT_DIR)

run-client: $(CLIENT)
	clear
	$(CLIENT) $(LOCAL) $(PORT)

# ---------- Server Build ---------- #
server: $(SERVER)

$(SERVER): $(OBJ_S)
	$(CC) -o $@ $^

$(S_OBJ_DIR)/%.o: $(SRC_DIR)%.c
	@if [ ! -e "$(S_OBJ_DIR)" ]; then \
		mkdir -p $(S_OBJ_DIR); \
	fi
	$(CC) $(CFLAGS) $(SERVER_D) $(INC_FLAG) -c $^ -o $@

# ---------- Client Build ---------- #
client: $(CLIENT)

$(CLIENT): $(OBJ_C)
	$(CC) -o $@ $^

$(C_OBJ_DIR)/%.o: $(SRC_DIR)%.c
	@if [ ! -e "$(C_OBJ_DIR)" ]; then \
		mkdir -p $(C_OBJ_DIR); \
	fi
	$(CC) $(CFLAGS) $(CLIENT_D) $(INC_FLAG) -c $^ -o $@

# ---------- Clean CMDs ---------- #
clean: clean-server clean-client

clean-server:
	@if [ -e "$(SERVER)" ] || [ -e "$(OBJ_S)" ]; then \
		echo "\033[31mRemoving client files\033[0m"; \
		echo "rm -f $(SERVER) $(OBJ_S)"; \
		rm -f $(SERVER) $(OBJ_S); \
	else \
		echo "Okay"; \
	fi

clean-client:
	@if [ -e "$(CLIENT)" ] || [ -e "$(OBJ_C)" ]; then \
		echo "\033[31mRemoving client files\033[0m"; \
		echo "rm -f $(CLIENT) $(OBJ_C)"; \
		rm -f $(CLIENT) $(OBJ_C); \
	else \
		echo "Okay"; \
	fi
