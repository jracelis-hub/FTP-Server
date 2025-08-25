# Socket Programming in C

## Overview

I found myself working between different devices under my network. I currently have my desktop and two Raspbery Pis that I tend to transfer different files from one play to another via using `scp`. 

With this project, I wanted to challenge myself in learning networking programming on a lower level. While there are many protocols and services to transfer files from one device to another `scp` `ftp` `ftps`... etc. I thought this would be a great learning experience to create my own file transfer program in C.

After gaining knowledge on the Linux OS and intercommunication that is happening between embedded devices on my current job, I thought this would a great way to test my skills but also strengthen my ability to understand networking and systems on a lower level.

## Sockets Background

What is a ***socket***?

A ***socket*** is a way to communicate between two programs via software, either on the same machine or across a network.

The sockets API in was released in 1983 under the 4.2BSD (_Berkeley Software Distribution_) Unix operating system.

The two main type of ***Socket*** domains are:

1.) ***Internet Domain Sockets (INET)*** - Used to communicate between process on different systems connected over a network via TCP/IP or UDP/IP. Utilizes the 2 main IPs, `ipv4` 32 bit and `ipv6` 128 bit.

2.) ***Unix Domain Sockets (UDS)*** - is an inter-process communication mechanism that allows bidirectional data exchange between process running on the same machine.

Types of ***Sockets***:

1.) ***Stream Sockets (SOCK_STREAM)*** - Provide a reliable, connection-oriented communication channel, ensuring that data is delivered in order and without dupliocation. Usually used with `TCP`. 

2.) ***Datagram Sockets (SOCK_DGRAM)*** - Offer a connectionless communication method, faster but does not guarantee order or delivery. Usually used with `UDP`. 

3.) ***Raw Sockets (SOCK_RAW)*** - Provides access to low-level network protocols and interfaces. Ordinary user programs usually have no need to use this style. 

**Server** - Provides a specific service depending to the client request, usually over the internet

**Client** - Request services or resources from a server over a network.

## Design Philosophy

Since I have been working with Unix based systems, I wanted to design this program to be as POSIX (Portable Operating System Interface) compliant as possible. Emphasising on portablility.

The network stack, consist of using **TCP** and will only support IPv4... but with a couple tweeks it could also support IPv6. The purpose of using **TCP** is to ensure the proper data is transfered over the network with consistent reliability. 

For handling multiple connections, I went with pthreads threads approach. Since, the amount requests client devices is less than 10 on my private network. This would satisfy the performance needs without being too overkill and on using other APIs like `select()` or `poll()`. If scalability was a requirement then I would go with an alternative approach.

Although encryption is essential when sending data over the internet for security and privacy, this program will not include it as the transfers will partake within my LAN (local area network).

![program flow](images/flow_chart.png "Program Flow Chart")

## Requirements

To run the following program a linux environment is needed. I have tested this on WSL (Windows Subsystem for Linux) and Debian-based Linux distributions such as Ubuntu and Raspbian (Raspberry Pi OS).

### Environment Setup

Methods:

#### WSL 

To install WSL on Windows open up the command prompt and type:
```
wsl --install
```
Set up password and other installation requirements as prompted.

Once installed, just type wsl on the command prompt and it will open the WSL terminal.

> [!NOTE]
> Also just use `window + r` and type wsl to open it.

Run the following script [update-system.sh](setup-scripts/update-system.sh) to make sure system is up to date.
```bash
chmod +x update-system.sh && ./update-system.sh
# Or
bash update-system.sh
```

To find the raspberry pi on the network I use the commandline utility `nmap`.

> [!NOTE]
> If nmap is not installed run the following code to install it on the system

```bash
sudo apt install nmap -y
```
Once install type `nmap -v` to see the version.

Example:
```
Starting Nmap 7.94SVN ( https://nmap.org ) at 2025-08-25 09:40 PDT
Read data files from: /usr/bin/../share/nmap
WARNING: No targets were specified, so 0 hosts scanned.
Nmap done: 0 IP addresses (0 hosts up) scanned in 0.01 seconds
```

Once everything is installed run the following to find which devices have port 22 (_ssh_) opened on the network
```
nmap -p 22 192.168.0.1/24 | grep -B 3 open
```

The following command will scan the whole host subnet looking for open (ssh) port 22 and capturing the following IPv4 address.
```
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


sample:
```bash
pifive@raspberrypi:~ $ ./manual-ip.sh
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

## Initial Testing V0 (Single Thread)

The [v0](v0/my_networking.c) was to create the bare skeleteon of the program itself to check for any error handling and to validate if the user input arguments are within the lengths of IPv4 and not utilizing any of the known ports below 1024. 

Although it does not catch proper formatting when entering the first command line argument for IPv4 in the form for of:

- 127.0.0.1
- 192.xxx.xxx.xxx
- 172.xxx.xxx.xxx
- 10.xxx.xxx.xxx

It does have a min and max value of the string length that the user can input for example:

- IP_MIN_LEN 10.0.0.0 = 8 + `'\0'` = 9
- IP_MAX_LEN 192.xxx.xxx.xxx = 15 + `'\0'` = 16

> [!NOTE]
> See [my_network.h](src/Version_0/my_networking.h) for defined macros.

My initial testing consist of:

- Proper command line 
See source code [version](src/Version_0/my_networking.c)

## Multi Thread 
