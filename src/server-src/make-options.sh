#!/usr/bin/env bash

RED="\033[31m"
RESET="\033[0m"

IPV4_REGREX='[0-9]{2,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}'

LOCAL='127.0.0.1'
LOCAL6='::1'
IPV4="$(hostname -I | grep -Eo ${IPV4_REGREX})"
PORT='2121'
DIRECTORY="$HOME"

SERVER=(run_local_server 
        run_local6_server 
        run_ipv4_server 
		build_server
		build_server6 
		exit_menu)

line_format()
{
	for i in {1..70}
	do
		echo -n '-'
	done
	echo
}

run_local_server()
{
	make clean
	make run-server INET=1 IP=${LOCAL} PORT=${PORT} MNT=${DIRECTORY}
}

run_local6_server()
{
	make clean
	make run-server INET6=1 IP=${LOCAL6} PORT=${PORT} MNT=${DIRECTORY}
}

run_ipv4_server()
{
	make clean
	make run-server INET=1 IP=${IPV4} PORT=${PORT} MNT=${DIRECTORY}
}

build_server()
{
	make clean
	make build-server INET=1
}

build_server6()
{
	make clean
	make build-server INET6=1
}

exit_menu()
{
	echo "Okay exiting menu..."
}

select_server_option()
{
	echo "Choose the following options to build or build and connect:"
	line_format
	echo -e "${RED}6${RESET} to ${RED}exit${RESET}"
	select option in "${SERVER[@]}"
	do
		echo "${REPLY}: Running ${option}"
		${option}
		break
	done
}

select_server_option
exit 0
