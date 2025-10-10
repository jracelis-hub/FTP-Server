#!/usr/bin/env bash

RED="\033[31m"
RESET="\033[0m"

LOCAL='127.0.0.1'
LOCAL6='::1'
PORT='2121'
IPV4="$1"

CLIENT=(connect_local_server 
		connect_local6_server 
        connect_network_server 
		build_client
		build_client6
        exit_menu)

line_format()
{
	for i in {1..70}
	do
		echo -n '-'
	done
	echo
}

connect_local_server()
{
	make clean
	make run-client INET=1 IP=${LOCAL} PORT=${PORT}
}

connect_local6_server()
{
	make clean
	make run-client INET6=1 IP=${LOCAL6} PORT=${PORT}
}

connect_network_server()
{
	make clean
	make run-client INET=1 IP=${IPV4} PORT=${PORT}
}

build_client()
{
	make clean
	make build-client INET=1
}

build_client6()
{
	make clean
	make build-client INET6=1
}

exit_menu()
{
	echo "Okay exiting menu..."
}

select_client_option()
{
	echo "Choose the following options to build or build and connect:"
	line_format
	echo -e "${RED}6${RESET} to ${RED}exit${RESET}"
	select option in "${CLIENT[@]}"
	do
		echo "${REPLY}: Running ${option}"
		${option}
		break
	done
}

select_client_option
exit 0
