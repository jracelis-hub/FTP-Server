# FTP Server

- [Features](#features)
- [Overview](#overview)
- [Design Philosophy](#design-philosophy)
- [Requirements](#requirements)
	- [Environment Setup](#environment-setup)
- [Initial Testing](#initial-testing-v0-single-thread)
- [Multi Thread](#multi-thread)
		
## Features

- [x] Multi-threaded 
- [x] TCP with IPv4 or IPv6 Support
- [x] Command Line Interface
- [x] File Transfer (text)
- [x] Half-Duplex
- [x] Server <-> Client Model
- [x] POSIX Compliant
- [x] File Transfer Size 4 KiB

- [] Windows
- [] Encryption

## Overview

I found myself working between different devices under my network. I currently have my desktop and two Raspbery Pis that I tend to transfer different files from one play to another via using `scp`. 

With this project, I wanted to challenge myself in learning networking programming on a lower level. While there are many protocols and services to transfer files from one device to another `scp` `ftp` `ftps`... etc. I thought this would be a great learning experience to create my own file transfer program in C.

## Design Philosophy

Since I have been working with Unix based systems, I wanted to design this program to be as POSIX (Portable Operating System Interface) compliant as possible. Emphasising on portablility.

The network stack, consist of using **TCP** and will only support IPv4... but with a couple tweeks it could also support IPv6. The purpose of using **TCP** is to ensure the proper data is transfered over the network with consistent reliability. 

For handling multiple connections, I went with pthreads threads approach. Since, the amount requests client devices is less than 10 on my private network. There are altnative approaches using event based APIs like `select()` or `poll()` for a more scalable approach.

Although encryption is essential when sending data over the internet for security and privacy, this program will not include it as the transfers be within my _LAN_ (local area network).

![program flow](images/flow_chart.png "Program Flow Chart")

The FTP (File Transfer Protocol) rules and details utilized for this project is found in [RFC 959](#https://datatracker.ietf.org/doc/html/rfc959#ref-1). 

The hosts are broken down to the client <-> server model. 

The server being the FTP server (replier) and client is the user (requestor).

The server will be listen on a its own configured port and will be _"mounted"_ around a specified directory on the server.

Using the base set of rules and structure, the flow consists of the Data Transfer Process (DTP) and Command Process (CP).

**Data Transfer Process** - the process of the server sending over requested information from the client(user).

**Command Process** - a set of commands that the client(user) requests to determine the correct data transfer process.

<table>
	<thead align="center">
		<tr>
			<th colspan="2">Command Table</th>
		</tr>
	</thead>
	<tbody>
		<tr>
			<th>Command</th>
			<th>Description</th>
		</tr>
		<tr>	
			<td>Upload</td>
			<td>to upload a file onto the server</td>
		</tr>	
		<tr>	
			<td>Download</td>
			<td>to download a file from the server to local</td>
		</tr>	
		<tr>	
			<td>List</td>
			<td>to all the files mounted directory on the server</td>
		</tr>	
		<tr>	
			<td>Read</td>
			<td>to read the information of a file from the server</td>
		</tr>	
	</tbody>
</table>

![request flow](images/request_diagram.png "Request Format")


## Requirements

To run the following program a linux environment is needed. I have tested this on WSL (Windows Subsystem for Linux) and Debian-based Linux distributions such as Ubuntu and Raspbian (Raspberry Pi OS).

The WSL2 version used:
```
wsl -v
WSL version: 2.5.9.0
Kernel version: 6.6.87.2-1
WSLg version: 1.0.66
MSRDC version: 1.2.6074
Direct3D version: 1.611.1-81528511
DXCore version: 10.0.26100.1-240331-1435.ge-release
Windows version: 10.0.26100.4946
```

### Environment Setup

Methods:

#### WSL 

To install WSL on Windows open up the command prompt and type:
```
wsl --install
```
Set up password and other installation requirements as prompted.

Once installed, just type wsl on the command prompt and it will open the WSL terminal.

> [!TIP]
> Also just use `window + r` and type wsl to open it.

Run the following script [update-system.sh](setup-scripts/update-system.sh) to make sure system is up to date.
```bash
chmod +x update-system.sh && ./update-system.sh
# Or
bash update-system.sh
```

To find the raspberry pi on the network I use the command line utility `nmap`. To scan 

To check if it is install type `nmap -v` to see the version. 

Example:
```bash
Starting Nmap 7.94SVN ( https://nmap.org ) at 2025-08-25 09:40 PDT
Read data files from: /usr/bin/../share/nmap
WARNING: No targets were specified, so 0 hosts scanned.
Nmap done: 0 IP addresses (0 hosts up) scanned in 0.01 seconds
```

> [!NOTE]
> If nmap is not installed no version will be shown. Run the following to install it on the system.

```bash
sudo apt install nmap -y
```

Once everything is installed run the following to find which devices have port 22 (_ssh_) opened on the network
```bash
nmap -p 22 192.168.0.1/24 | grep -B 4 open
```

The following command will scan the whole host subnet looking for open (ssh) port 22 and capturing the following IPv4 address.
```bash
Nmap scan report for 192.168.0.135
Host is up (0.055s latency).

PORT   STATE SERVICE
22/tcp open  ssh
--
Nmap scan report for 192.168.0.245
Host is up (0.052s latency).

PORT   STATE SERVICE
22/tcp open  ssh
--
Nmap scan report for 192.168.0.250
Host is up (0.021s latency).

PORT   STATE SERVICE
22/tcp open  ssh
```

Since, my system DHCP server range is only from 192.168.0.2 - 192.168.0.200. I can determine which device was recently added to my _LAN_. 

From there I ssh into to the Pi to get my system all setup. For the [Raspberry Pi](#raspberry-pi) section for setup.

#### Raspberry Pi

<table>
	<thead align="center">
		<tr>
			<th colspan="2">Build of Materials</>
		</tr>
		<tr>
			<th>Quantity</th>
			<th>Description</th>
		</tr>
	</thead>
	<tbody align="center">
		<tr>
			<td>1</td>
			<td>Raspberry Pi 5</td>
		</tr>
		<tr>
			<td>1</td>
			<td>Power Supply 5V 5Amps</td>
		</tr>
		<tr>
			<td>1</td>
			<td>Micro SD Card 32GB+</td>
		</tr>
	</tbody>
</table>

To get the raspberry pi up and running

Sample:
```bash
pifive@raspberrypi:~ $ chmod +x manual-ip.sh && ./manual-ip.sh
Beginning manual configuration...
Enter IPv4 address: 192.168.0.251/24
Enter gateway address: 192.168.0.1
Enter DNS address: 8.8.8.8 1.1.1.1
IP = 192.168.0.251/24
Gateway = 192.168.0.1
DNS = 8.8.8.8 1.1.1.1
Does the follow information look correct [y/n]? y
Changing ipv4.addresses to 192.168.0.251/24...
done
Changing ipv4.gateway to 192.168.0.1...
done
Changing ipv4.dns to 8.8.8.8 1.1.1.1...
done
Changing ipv4.method to manual...
done
Resetting network...
```

Ubuntu

Raspbian

## Initial Testing V0 Single Thread

v0 format file structure is broken down into the following format:
```
├── Makefile
├── build
│   ├── c_obj
│   │   ├── client.o
│   │   ├── main.o
│   │   └── net_utility.o
│   ├── s_obj
│   │   ├── commands.o
│   │   ├── main.o
│   │   ├── net_utility.o
│   │   └── server.o
│   ├── start-client
│   └── start-server
├── include
│   ├── commands.h
│   └── net_utility.h
└── src
    ├── client.c
    ├── commands.c
    ├── main.c
    ├── net_utility.c
    ├── server.c
    └── unit-test.c
```

The [v0](v0/my_networking.c) was to create the bare skeleteon of the program itself to check for any error handling and to validate if the user input arguments are within the lengths of IPv4 and not utilizing any of the known ports below 1024. 

> [!NOTE]
> See [my_network.h](src/Version_0/my_networking.h) for defined macros.

My initial testing consist of:

- Proper command line 
See source code [version](src/Version_0/my_networking.c)

## Multi Thread 
