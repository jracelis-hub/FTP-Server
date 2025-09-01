#!/usr/bin/env bash

RESET="\033[0m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"

update-system () {
	echo -e "${YELLOW}Updating system...${RESET}"
	sudo apt-get update -y
	echo -e "${YELLOW}Upgrading system...${RESET}"
	sudo apt-get upgrade -y
}

update-system
