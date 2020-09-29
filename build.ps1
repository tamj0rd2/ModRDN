$getVmState = '& VBoxManage.exe showvminfo "windows xp" | Select-String -Pattern "State:" | Out-String'

function OnBuildComplete {
  Write-Host 'Pausing the vm' -ForegroundColor 'blue'
  & 'D:\Program Files\Oracle\VirtualBox\VBoxManage.exe' controlvm "windows xp" pause
  
  Write-Host 'Script complete!' -ForegroundColor 'green'
}

Write-Host "Connecting to VirtualBox..." -ForegroundColor 'blue'
$initialVmState = Invoke-Expression $getVmState

if ($initialVmState.Contains('paused')) {
  Write-Host "Resuming the VM" -ForegroundColor 'blue'
  & 'D:\Program Files\Oracle\VirtualBox\VBoxManage.exe' controlvm "windows xp" resume
}

if ($(Invoke-Expression $getVmState).Contains('running')) {
  Write-Host "Trying to build the code..." -ForegroundColor 'blue'
  VBoxManage.exe guestcontrol "windows xp" run -- "Z:\ModRDN\build-guest-code.bat"
  OnBuildComplete
  exit 0
}

if ($initialVmState.Contains('powered off')) {
  # disable password requirement: control userpasswords2 > untick checkbox
  Write-Host "Powering on the VM" -ForegroundColor 'blue'
  & 'D:\Program Files\Oracle\VirtualBox\VBoxManage.exe' startvm "windows xp" --type headless
  Write-Host "Going to connect to the VM and build the code. Be patient." -ForegroundColor 'blue'
  for ($attemptNo = 0; $attemptNo -lt 21; $attemptNo++) {
    $output = & VBoxManage.exe guestcontrol "windows xp" run -- "Z:\ModRDN\build-guest-code.bat" 2>&1 | Out-String
  
    if (!$output.Contains('VBoxManage.exe: error')) {
      Write-Host $output
      break
    }
  
    if ($attemptNo -eq 20) {
      Write-Host "Problem start/connecting to the VM" -ForegroundColor 'red'
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
