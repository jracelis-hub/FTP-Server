#!/usr/bin/env bash

RESET="\033[0m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"

update-system() 
{
	echo -e "${YELLOW}Updating system...${RESET}"
	sudo apt-get update -y
	sleep 1
	echo -e "${YELLOW}Upgrading system...${RESET}"
	sudo apt-get upgrade -y
	sleep 1
	echo -e "Installing Package: ${BLUE}git${RESET}"
	sudo apt-get install git -y
	sleep 1
	echo -e "Installing Package: ${BLUE}gcc${RESET}"
	sudo apt-get install gcc -y
	sleep
	echo -e "Installing Package: ${BLUE}make${RESET}"
	sudo apt-get install make -y
	echo -e "Installation ---------------> ${GREEN}Done${RESET}"
}

update-system
