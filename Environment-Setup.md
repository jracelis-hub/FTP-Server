# Environment Setup

- [WSL](#wsl)
- [Virtual Machine](#virtual-machine)
- [Raspberry Pi](#raspberry-pi)

### WSL 

To install WSL on Windows open up the command prompt and type:
```
wsl --install
```
Set up password and other installation requirements as prompted.

Once installed, just type wsl on the command prompt and it will open the WSL terminal.

The WSL2 version used:
```bash
wsl -v
WSL version: 2.5.9.0
Kernel version: 6.6.87.2-1
WSLg version: 1.0.66
MSRDC version: 1.2.6074
Direct3D version: 1.611.1-81528511
DXCore version: 10.0.26100.1-240331-1435.ge-release
Windows version: 10.0.26100.4946
```

> [!TIP]
> Also just use `window + r` and type wsl to open it.

Run the following script [update-system.sh](setup-scripts/update-system.sh) to make sure system is up to date.
```bash
cd FTP-Server/setup-scripts && chmod +x update-system.sh && ./update-system.sh
```
Or
```bash
cd FTP-Server/setup-scripts && bash update-system.sh
```

### Virtual Machine

The two options that I will highlight is either:
1. [Orcale Virtual Box](https://www.virtualbox.org/wiki/Downloads)
2. Hyper-V

Go to the following drop down to get software up in running but in the mean time download the following Linux Distrobution you plan on using.
- [Ubuntu](https://ubuntu.com/download/desktop/thank-you?version=24.04.3&architecture=amd64&lts=true) 24.04.3 LTS

<details>
<summary>Orcale Virtual Box</summary>

To set up Orcale Virtual Box go through the steps by clicking the following link [Orcale Virtual Box Download](https://www.virtualbox.org/wiki/Downloads)

Click the following download based on the type of Operating System.

Example: Click [here](https://download.virtualbox.org/virtualbox/7.2.2/VirtualBox-7.2.2-170484-Win.exe) for the windows download.

Once download is complete and installed, go through the steps of creating a virtual machine.

Click `New` and input the following information:
- `VM Name` Input Information
- `ISO Image` Add which ever you are using [Ubuntu](https://ubuntu.com/download/desktop/thank-you?version=24.04.3&architecture=amd64&lts=true)
- `User Name` & `Password`
- `Hostname`
- Allocate `RAM` `CPU` and `Disk` space
Hit finish

From there boot up the system.

Once booted up in the termial do the following:
```bash
git clone https://github.com/jracelis-hub/FTP-Server.git
```
Run the following script [update-system.sh](setup-scripts/update-system.sh) to make sure system is up to date.
```bash
cd setup-scripts && chmod +x update-system.sh && ./update-system.sh
```
Or
```bash
cd setup-scripts && bash update-system.sh
```
---
</details>

<details>
<summary>Hyper V</summary>

For offical documentation of how to install Hyper V use [Install Hyper-V](https://learn.microsoft.com/en-us/windows-server/virtualization/hyper-v/get-started/install-hyper-v?tabs=gui&pivots=windows).

To download through gui version which I will be highlighting go start menu or `window` + `r` and type control panel 
- Control Panel -> Programs and Features
- Left side click `Turn Windows features on or off`
- Scroll down to `Hyper V` and check the box off
- Reboot may be required

Open up `Hyper-V Manager` and for quick installation do the following:
- `Quick Create...` on the right side of the task bar
- Click `Ubuntu LTS` 
- Create Virtual Machine

> Note: The LTS support may not be up to date download from [Ubuntu](https://ubuntu.com/download/desktop/thank-you?version=24.04.3&architecture=amd64&lts=true) for the up to date version.

Walk through the set up once set up do the following:
```
git clone https://github.com/jracelis-hub/FTP-Server.git
```
Run the following script [update-system.sh](setup-scripts/update-system.sh) to make sure system is up to date.
```bash
cd setup-scripts && chmod +x update-system.sh && ./update-system.sh
```
Or
```bash
cd setup-scripts && bash update-system.sh
```
---
</details>

> [!NOTE]
> But if using Hyper-V might as well take the extra step and install `wsl`

### Raspberry Pi

#### Build of Materials

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

**Installation**

To get the Raspberry Pi up and running go to [Raspberry Pi](https://www.raspberrypi.com/software/) to download the RPi Imager and choose based on the system:
- [Windows](https://downloads.raspberrypi.com/imager/imager_latest.exe)
- [Mac](https://downloads.raspberrypi.com/imager/imager_latest.dmg)

Plug the micro SD card into computer and walk through the RPi Imager software
- `Choose Device` Raspberry Pi 5... etc.
- `Choose OS` Raspberry Pi OS -> Raspberry Pi OS Lite (64-bit)
- `Choose Storage` and change configuration information
	- Input hostname info
	- Set up Wifi info
	- Enable `SSH`

Once everything is set up and flashed. Plug in micro SD card and power on RPi.

The RPi should show up on the network after a couple of minutes.

To find the RPi on the network I use the command line utility `nmap`. To scan my subnet mask.

> [!WARNING]
> Only do `nmap` in a network that is allowed to be perfomed on, `nmap` can raise security risks when under a network that is not your own.

To check if it is install type `nmap -v` to see the version:
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

Once everything is installed run the following to find which devices have port 22 (_ssh_) opened on the network.
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
Since, my network DHCP server range is only from 192.168.0.2 - 192.168.0.200. I can determine which device was recently added to my _LAN_. 

> I did this through changing the DHCP settings on my COX router/moderm via app, procedure may vary.

Now get the repository on the RPi
```bash
git clone https://github.com/jracelis-hub/FTP-Server.git
```

From there I ssh into to the Pi to get my system all setup. Running the following [update-system.sh](setup-scripts/update-system.sh).
```bash
cd FTP-Server/setup-scripts && chmod +x update-system.sh && ./update-system.sh
```

or

```bash
cd FTP-Server/setup-scripts/ && bash update-system.sh
```

And to set up a static IP using [setup-static-ip.sh](setup-scripts/update-system.sh)
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
From there the RPi will boot up and ssh back in with the new IP address.

To check that it has changed run:
```bash
hostname -I | grep -oE '[0-9]{2,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'
# This should show new IP address
192.168.0.251
```