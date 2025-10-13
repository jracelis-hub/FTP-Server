#!/usr/bin/env bash

# Color text
RESET="\033[0m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"

# Pulls name of the network information for wlan0
NET_NAME="$(nmcli c show | awk '/wlan0/ {print $1}')"

get_ip() 
{
        echo -en "${YELLOW}Enter IPv4 address:${RESET} "
        read -e -i "192.168.0.xxx/24" IP
}

get_gateway() 
{
        echo -en "${YELLOW}Enter gateway address:${RESET} "
		# Values are preset use arrow keys to change value
        read -e -i "192.168.0.1" GATEWAY
}

get_dns() 
{
        echo -en "${YELLOW}Enter DNS address:${RESET} "
		# Values are preset use arrow keys to change value
        read -e -i "8.8.8.8 1.1.1.1" DNS
}

# Double check if the following information is good
verify() 
{
        echo -e "IP = ${BLUE}${IP}${RESET}"
        echo -e "Gateway = ${BLUE}${GATEWAY}${RESET}"
        echo -e "DNS = ${BLUE}${DNS}${RESET}"
        echo -n "Does the follow information look correct [y/n]? "
        read INPUT
}

# To set up a static IP address
# Note the ip address will only be static if DHCP range is not within the following IP addresses
manual_mode () {
        echo "Changing ipv4.addresses to ${IP}..."
        sudo nmcli connection modify "${NET_NAME}" ipv4.addresses "${IP}"
        echo "done"
        echo "Changing ipv4.gateway to ${GATEWAY}..."
        sudo nmcli connection modify "${NET_NAME}" ipv4.gateway "${GATEWAY}"
        echo "done"
        echo "Changing ipv4.dns to ${DNS}..."
        sudo nmcli connection modify "${NET_NAME}" ipv4.dns "${DNS}"
        echo "done"
        echo "Changing ipv4.method to manual..."
        sudo nmcli connection modify "${NET_NAME}" ipv4.method "manual"
        echo "done"
}

reset_connection() 
{
        echo -e "Resetting network..."
        sudo nmcli connection down "${NET_NAME}"
        sudo nmcli connection up "${NET_NAME}"
}

verbose_mode() 
{
        echo -e "${YELLOW}Beginning manual configuration...${RESET}"
        echo -e "${RED}Use the arrow keys to change the values!${RESET}"
        get_ip
        get_gateway
        get_dns
        verify
        if [[ "${INPUT}" == "y" || "${INPUT}" == "y" ]]; then
                manual_mode
                reset_connection
        else
                echo "Okay exiting..."
                exit 0
        fi
}

verbose_mode
exit 0
