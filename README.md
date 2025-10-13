# FTP Server

> [!CAUTION]
> Working In Progress

- [Features](#features)
- [Overview](#overview)
- [Design Philosophy](#design-philosophy)
	- [Architecture Overview](#command-process-overview)
	- [Command Process](#command-process-overview)
	- [Memory Layout](#memory-layout)
- [Requirements](#requirements)
- [How To](#how-to)
	- [Build](#build)
	- [Run](#run)
- [Testing](#testing)
		
## Features

Haves
- [x] Multi-threaded 
- [x] TCP with IPv4 or IPv6 Support
- [x] Command Line Interface
- [x] File Transfer (text)
- [x] Half-Duplex
- [x] Server <-> Client Model
- [x] POSIX Compliant
- [x] File Transfer Size 4 KiB

Wants
- [ ] Windows
- [ ] Encryption

## Overview

I found myself working between different devices under my network. I currently have my desktop and two Raspbery Pis that I tend to transfer different files from one play to another via using `scp`. 

With this project, I wanted to challenge myself in learning networking programming on a lower level. While there are many protocols and services to transfer files from one device to another `scp` `ftp` `ftps`... etc. I thought this would be a great learning experience to create my own file transfer program in C.

## Design Philosophy

Since I have been working with Unix based systems, I wanted to design this program to be POSIX (Portable Operating System Interface) compliant.

The network stack, consist of using `TCP`, that supports either `IPv6` or `IPv4`. The purpose of using `TCP` is to ensure data is transfer over the network consistly and reliablily over the network.

For handling multiple connections, I went with `pthreads` (POSIX threads). Since, the amount requests client devices is less than 10 on my private network. There are altnative approaches using event based APIs like `select()` or `poll()` for a more scalable approach.

Although encryption is essential when sending data over the internet for security and privacy, this program will not include it as the transfers be within my _LAN_ (local area network).

<p align="center">
	<img src="images/flow_chart.png" title="Program Flow Chart">
	<b>Figure 1</b>: Design Flow
</p>

The FTP (File Transfer Protocol) concepts and details utilized for this project is found in [RFC 959](https://datatracker.ietf.org/doc/html/rfc959#ref-1). 

The hosts are broken down to the client <-> server model. 

The server being the FTP server (`replier`) and client is the user (`requestor`).

The server will be listen on a its own configured port and will be ***"mounted"*** around a specified directory on the server.

Using the base set of rules and structure, the flow consists of the **Data Transfer Process** and **Command Process**.

**Data Transfer Process** - the process of the server sending over requested information from the client (user).

**Command Process** - a set of commands that the client (user) requests to determine the correct data transfer process.

<table align="center">
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

### Architecture Overview

<p align="center">
	<img src="./images/ftp_architecture.png" title="FTP Architecture">
	<b>Figure 2</b>: FTP Architecture
</p>

### Command Process

The command process has 5 key components:
1. **Request** - the `request` represents the command process as a whole, always containing the header and the command type.
2. **Header** - the header holds the information of what the server needs to conduct data transfer process. Header always has a command but doesn't always have a file/file path.
3. **Command** - the command is a command process that lets the server know which data transfer needs to occur when replying back to the client.
4. **File** - the file or the file path that the server conducts the data transfer process on.
5. **Payload** - the payload is the data that is being transfer to `upload` onto the server.

<p align="center">
	<img src="./images/request_diagram.png" title="Command Format">
	<br></br>
	<b>Figure 3</b>: Command Format
</p>

### Memory Layout

## Requirements

Tested on the following versions:
- GCC `gcc 13.3.0`
- Make `gnu make 4.3`
- Git `git version 2.43.0`
- WSL (Optional) `WSL version: 2.5.9.0`
- Ubuntu (Optional) `ubuntu version 24.04.3 LTS`

> [!IMPORTANT]
> The following testing was conducted in a Linux environment. With the usage of `pthreads` POSIX threads. To have the best results use WSL, Ubuntu or Raspbian (RPi OS). 

If not in a Linux environment go to [environment setup](Environment-Setup.md).

## How To

### Build

The build directory consists of 3 main directories:
- server source code
- client source code
- header files

> See drop down directory overview

<details>
<summary><b>Directory Layout</b></summary>

Directories Layout:
- `client-src` - holds the source code to for the client side that connects to the server
- `server-src` - holds the source code to run the server on the machine of choice
- `include` - holds the header files of both `client-src` and `server-src`

After building the following 2 directories get created:
- `build` - holds the executable to run
- `obj` - holds the object files per source files when compiling

```shell
├── client-src    <--------------  client directory
│   ├── Makefile  <--------------  make file
│   ├── make-options.sh   <------  bash script 
│   ├── client.c          <------
│   ├── commands-client.c       |
│   ├── logging.c               |  src files
│   ├── main.c                  |
│   ├── parsing.c         <------
│   ├── build
│   │   └── client  <------------- executable 
│   └── obj
│       ├── client.o          <---
│       ├── commands-client.o    |
│       ├── logging.o            | object files
│       ├── main.o               |
│       └── parsing.o         <---
├── include   <------------------- header directory
│   ├── client.h          <-------
│   ├── command_type.h           |
│   ├── commands-client.h        |
│   ├── commands-thread.h        | header files
│   ├── logging.h                |
│   ├── parsing.h                |
│   ├── server.h                 |
│   └── thread.h          <-------
└── server-src
    ├── Makefile
    ├── make-options.sh
    ├── commands-thread.c
    ├── logging.c
    ├── main.c
    ├── parsing.c
    ├── server.c
    ├── thread.c
    ├── build
    │   └── server
    └── obj
        ├── commands-thread.o
        ├── logging.o
        ├── main.o
        ├── parsing.o
        ├── server.o
        └── thread.o
```

---
</details>

When compiling there are two ways to compile:
1. Manual running `make`
2. Using `./make-options.sh`

**Option 1**

Use the following `make` arguments to build:
- `make build-server INET=1` - to build IPv4 server executeable (verbose)
- `make build-server INET6=1` - to build IPv6 server executable (verbose)
- `make INET=1` - to build IPv4 server exectuable
- `make INET6=1` - to build IPv6 server executable

```shell
# Creates executable in ./build and run it via ./build/server
make build-server INET=1 
# Making a IPv6 server
make build-server INET6=1
# Less verbose IPv4
make INET=1 
# Less verbose IPv6
make INET6=1 
```

**Option 2**

Run the following shell script `./make-options.sh`
- `4) build_server` - to build IPv4 server exectuable
- `5) build_server6` - to build IPv6 server exectuable

> [!NOTE]
> I am not mentioning `1-3` as that is an automated way to build and run, this will be shown in [How To Run](#run) section.

```shell
# As prompted just choose the following output when running 
./make-options.sh
Choose the following options to build or build and connect:
----------------------------------------------------------------------
6 to exit
1) run_local_server
2) run_local6_server
3) run_ipv4_server
4) build_server
5) build_server6
6) exit_menu
#? 4 
```

### Run

Server:

The `./server` takes 3 arguments:
1. IPv4 or IPv6 address that will be hosting the server.
2. Port that the server will be listening on.
3. Mounted directory that the server will be available for the client to use.

When running exectuable in wrong order or incorrect arguments `usage()` will pop up on proper parameters. See [Error Logging](#error-logging) for output.

```shell
# Home is /home/user_directory. The directory can be any directory
cd build && ./server 127.0.0.1 2121 $HOME
```

To use the automated version use `make-options.sh` and choose the following `1-3` choices as seen above.
1. `run_local_server` - binds to IPv4 127.0.0.1 on port 2121
2. `run_local6_server` - binds to IPv6 ::1 on port 2121
3. `run_ipv4_server` - uses `hostname -I` to grab IPv4 (192.168.0.xxx) address on port 2121

> IPv4 `hostname -I` is different per machine

```shell
# Build && run do the following
make run-server INET=1 
# This runs the default IPv4 IP=127.0.0.1 PORT=2121 MNT=$HOME
```

Client:

Like the server, the client takes similar arguments but instead of hosting, the client is trying to connect to the server interface.

The `./client` takes 2 arguments:
1. IPv4 or IPv6 address the server is hosting on that the client wants to connect to.
2. Port that the server is listening on for the client to connect.

```shell
# This is for connecting onto the local address
cd build && ./client 127.0.0.1 2121
```

To use the automated version use `make-options.sh` and choose the following `1-3` choices:
1. `connect_local_server` - connect to IPv4 127.0.0.1 on port 2121
2. `connect_local6_server` - connect to IPv6 ::1 on port 2121
3. `connect_network_server` - connect to IPv4 network address 

> Using option 3 needs an argument passed in before running `make-options.sh`

```shell
./make-options.sh 192.168.0.251
```

The manual operation using `make` to build and run the script with the following preset parameters:
```shell
# This runs the default IPv4 IP=127.0.0.1 PORT=2121
make run-client INET=1
```

> See the drop down for a demo on how to use.

<details>
<summary><b>Local Run demo</br></summary>
<br></br>
	<p align="center">
		<img src="videos/local_run_demo.gif" title="Local Run Demo">
	</p>

---
</details>

<details>
<summary><b>Raspberry Pi Run Demo</b></summary>
<br></br>
	<p align="center">
		<img src="videos/rpi_run_demo.gif" title="RPi Demo">	
	</p>

---
</details>

## Testing

### Error Logging 

All error logging is found in the header `logging.h` using `_error_msg()` and `usage()` to capture error codes and improper usage.

`usage()` shows usage message and an example how to use.
```bash
~/dev/FTP-Server/src/client-src/build $ ./client 2121
----------------------------------------
  Usage: ./client [IPv4] [Port].
Example: ./client 192.168.0.xxx 8000.
   Note: Port has to be 1023 > port < 65535. See /etc/protocols.
----------------------------------------
```

`_error_msg()` contains information on:
- File - the file in which it failed in
- Function - the function that it failed in
- Line - the line the `_error_msg()` occured in
- Description - a description of the error

```bash
~/dev/FTP-Server/src/client-src $ ./build/client 2600:8802:2f8c:6c00:4e63:5e1:19cf:5848 2121
Creating socket...
-----------------------------------> Done
Attempting to connect to server...
     Status: Error
       File: main.c
   Function: main
       Line: 53
Description: Could not connect to server
```

### Command
