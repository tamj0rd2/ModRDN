# ModRDNDevelopment

I'm having a mess around with the RockPaperScissors mod included with the Impossible Creatures SDK. I thought I'd document all the steps required to set up a development environment, compile and get the mod working in-game. This documentation is WIP

## Contents

- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Compiling the dll](#compiling-the-dll)
  - [Preparing the mod files](#preparing-the-mod-files)
  - [Troubleshooting](#troubleshooting)
- [Mod help](#mod-help)
- [The fun coding bits](#the-fun-coding-bits)
  - [Using this code](#using-this-code)
  - [Compiling conveniently from your host](#compiling-via-the-commandline)

## Getting started

### Prerequisites

- [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
- Windows XP bootable iso
- Visual Studio 2002 installation CDs

### Installation

1. Create a Windows XP virtual machine in VirtualBox with at least 10GB VDI
1. Add the bootable Windows XP iso to the machine's disk drive
1. Boot the virtual machine and install windows (do not enable updates)
1. Install DaemonTools
1. Mount the first Visual Studio CD and start the install (note, you won't be able to mount a ccd if the folder it exists in is mapped via a "shared folder" in virtual box. You need to copy the files somewhere in the vm like the Desktop or My Documents first)
1. When asked to install "additional components", mount disk number 5
1. Continue the installation, mounting the other CDs when asked (if aske)
1. Install the service pack

### Compiling the dll

1. Copy the SDK folder from your root IC installation directory into the VM
1. Open the RDNRelease solution
1. Switch the build configuration to `Release`
1. Build the solution
1. You shouldn't get any errors at all. The Dll will output to SDK\Obj\bin\RDNMod.dll

### Preparing the mod files

1. Temporarily install the RockPaperScissors mod from steam workshop
1. Copy all of the files from that mod's content into the IC root directory
1. Delete RDNMod.dll and replace it with the one you have compiled
1. Presto - the mod should show up in IC

### Troubleshooting

1. check `warnings.log` in the IC root directory to check for errors/warnings. That should hopefully give an idea if anything is wrong
1. Don't keep backup files in the IC root directory. For example, if you want to create a backup of the file `MyMod.dll`, do not rename it to `MyMod.dll.old`. It causes weird things to happen and I don't know why. Instead, keep your backups somewhere else outside of the IC root dir.

## Mod help

### How do I get my mod to be recognised by IC?

#### The files involved

If your mod is called `ModName`, you'd need these files

| File/Folder                      | Description                                                           | Notes                   |
|----------------------------------|-----------------------------------------------------------------------|-------------------------|
| modname.module                   | Contains metadata about your mod and where required files are located | should be all lowercase |
| ModName.dll                      | The compiled DLL for your mod                                         |                         |
| ModName                          | Put a Data folder in here that mirrors <ICRootDir>/Data               |                         |
| ModNameData.sga                  |                                                                       |                         |
| Locale\English\RDNMod\modloc.sga |                                                                       |                         |
| Locale\English\RDNMod\ModText.*  |                                                                       |                         |
  
#### The content of <ModName.module>

## The fun coding bits

### Using this code

1. Fork this project in github
2. Open your terminal/cmd
3. CD into <ICInstallationDirectory>/SDK
5. `git clone <your-fork-url> ModRDNDevelopment` (you can get this by clicking the Code dropdown button in github)
6. That should create a ModRDNDevelopment folder next to the original one

### Compiling via the commandline

Here's a very nice little workflow to compile the code all from your host machine. There's just a bit of
setup that you need to do the very first time.

1. Enable logging into the VM without a password:
    1. open the group policy editor on the command line by typing `gpedit.msc`
    2. open the key `Computer Configuration\Windows Settings\Security Settings\Local Policies\Security Options`
    3. change the value of `Accounts: Limit local account use of blank passwords to console logon only` to `Disabled`.
2. Allow logging into the VM without having to enter a password
    1. Go to Start > Run
    2. Type `control userpasswords2` and click ok
    3. Untick `Users must enter a user name and password to use this computer`
3. Add a permanent shared folder to the VM
    1. Go to your VM's settings > Shared folders
    2. Click the Add button
    3. Set the folder path to <ICInstallationDirectory>/SDK
    4. Tick `Auto-mount`
    5. Set the Mount Point to `z:\`
    6. Tick `Make Permanent`
    7. Make sure that within the VM you are able to browse to `Z:\ModRDNDevelopment`
4. Add `VBoxManage.exe` to your host machine PATH. It is located in the VirtualBox installation folder

All done! Now any time you want to build the code, you just need to run `./build.ps1` from powershell on
your host machine, assuming you are already CD'd into this ModRDNDevelopment directory

## Lessons learned

### Playing maps other than test_map

In the beginning, only test_map was playable using this mod. test_map is comprised of 2 HQs and some cashpiles.
The HQ allows you to spawn Rock, Paper and Scissor units which use the henchmen model. Standing next to cashpiles gives
you cash.

Playing on other maps didn't seem to work. "Not enough labs" would be shown in warnings.log. Attempted solutions:

- Creating a custom map with 2 HQs on it was successful. The map showed in game and could be played.
- Creating a custom map with 2 labs on it was not successful. The game still complain about not having enough labs
- The previous bullet point + changing the HQ ControllerType to 5 (which is the correct controller
  type for the lab ebp) did not work. The map would not show up in the map selection dropdown list
- The previous bullet point + changing HQ (`RDNEBPs.cpp`) to point to structures\lab instead of structures\HQ worked :D

And of course, the map will never load successfully unless all of the required EBPs are inside of
RDNMod\Data\art\ebps\structures etc

I went through a similar process with coal

- added coal to the map
- map would not show in selector
- added ebps for coal to the mod
- map showed but pwaited
- changed cashpile controller type and set the correct EBP path - all good!

## Glossary

ICInstallationDirectory: The folder where IC is installed

EBP: Entity blueprint

EC: Entity Controller

Root: <ICInstallationDirectory>\data

Ted: Object Editor
