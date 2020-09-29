# ModRDN

## Compiling via the commandline

1. CD into SDK/ModRDN

`cd Z:\ModRDN && Z:` (In my case I've mounted the SDK folder to the Z drive)

2. Initialise the visual studio command line prompt

`%comspec% /k "C:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat"`

3. Build the solution in release mode

`devenv RDNRelease.sln /build Release`
