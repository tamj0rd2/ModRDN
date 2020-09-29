# ModRDN

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
  - [Compiling via the commandline](#compiling-via-the-commandline)

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

THe below steps need to be done inside your windows xp vm

1. CD into SDK/ModRDN

`cd Z:\ModRDN && Z:` (In my case I've mounted the SDK folder to the Z drive)

2. Initialise the visual studio command line prompt

`%comspec% /k "C:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat"`

3. Build the solution in release mode

`devenv RDNRelease.sln /build Release`
