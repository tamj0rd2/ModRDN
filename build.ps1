$vmName = "windows xp"
$getVmState = "& VBoxManage.exe showvminfo '$vmName' | Select-String -Pattern 'State:' | Out-String"
$guestBuildScriptLocation = "Z:\ModRDNDevelopment\build-guest-code.bat"
$icInstallDirectory = "../.."
$icSdkDirectory = "$icInstallDirectory/SDK"
$dllInstalledPath = "$icInstallDirectory/RDNMod.dll"
$dllOutputPath = "$icSdkDirectory/Obj/bin/RDNMod.dll"

function OnBuildComplete {
  Write-Host 'Pausing the vm' -ForegroundColor 'blue'
  & VBoxManage.exe controlvm $vmName pause
  
  if (Test-Path $dllInstalledPath) {
    Remove-Item $dllInstalledPath
  }

  mv $icSdkDirectory/Obj/bin/RDNMod.dll $icInstallDirectory/RDNMod.dll
  Write-Host "Mod installed to $dllInstalledPath" -ForegroundColor 'green'
}

$initialVmState = Invoke-Expression $getVmState

if ($initialVmState.Contains('paused')) {
  Write-Host "Resuming the VM" -ForegroundColor 'blue'
  & VBoxManage.exe controlvm $vmName resume
}

if ($(Invoke-Expression $getVmState).Contains('running')) {
  Write-Host "Trying to build the code..." -ForegroundColor 'blue'
  VBoxManage.exe guestcontrol $vmName run -- $guestBuildScriptLocation
  OnBuildComplete
  exit 0
}

if ($initialVmState.Contains('powered off') -or $initialVmState.Contains('aborted')) {
  & VBoxManage.exe startvm $vmName --type headless

  Write-Host "Going to connect to the VM and build the code. This can take a minute or two because the VM is cold booting" -ForegroundColor 'blue'
  for ($attemptNo = 0; $attemptNo -lt 21; $attemptNo++) {
    $output = & VBoxManage.exe guestcontrol $vmName run -- $guestBuildScriptLocation 2>&1 | Out-String
  
    if (!$output.Contains('VBoxManage.exe: error')) {
      Write-Host $output
      break
    }
  
    if ($attemptNo -eq 20) {
      Write-Host "Problem starting/connecting to the VM" -ForegroundColor 'red'
      Write-Host $output -ForegroundColor 'red'
      exit 1
    }
  
    Start-Sleep -Seconds 1
  }

  OnBuildComplete
  exit 0
}


Write-Host "Problem trying to start the vm\n" $initialVmState -ForegroundColor 'red'
exit 1
