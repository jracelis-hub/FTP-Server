#!/usr/bin/env bash

RESET="\033[0m"
RED="\033[031m"
GREEN="\033[032m"
YELLOW="\033[033m"
BLUE="\033[034m"

update-system () {
	echo -e "${YELLOW}Updating system...${RESET}"
	sudo apt-get update -y
	echo -e "${YELLOW}Upgrading system...${RESET}"
	sudo apt-get upgrade -y
}

update-system
