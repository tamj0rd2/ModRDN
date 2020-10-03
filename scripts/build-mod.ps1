$settings = (python ./scripts/settings.py | ConvertFrom-Json)
$getVmState = "& VBoxManage.exe showvminfo '$($settings.vmName)' | Select-String -Pattern 'State:' | Out-String"

function OnBuildComplete {
  Write-Host 'Pausing the vm' -ForegroundColor 'blue'
  & VBoxManage.exe controlvm $settings.vmName pause
}

$initialVmState = Invoke-Expression $getVmState

if ($initialVmState.Contains('paused')) {
  Write-Host "Resuming the VM" -ForegroundColor 'blue'
  & VBoxManage.exe controlvm $settings.vmName resume
}

if ($(Invoke-Expression $getVmState).Contains('running')) {
  Write-Host "Trying to build the code..." -ForegroundColor 'blue'
  VBoxManage.exe guestcontrol $settings.vmName run -- $settings.guestBuildScriptLocation
  OnBuildComplete
  exit 0
}

if ($initialVmState.Contains('powered off') -or $initialVmState.Contains('aborted')) {
  & VBoxManage.exe startvm $settings.vmName --type headless

  Write-Host "Going to connect to the VM and build the code. This can take a minute or two because the VM is cold booting" -ForegroundColor 'blue'
  for ($attemptNo = 0; $attemptNo -lt 21; $attemptNo++) {
    $output = & VBoxManage.exe guestcontrol $settings.vmName run -- $settings.guestBuildScriptLocation 2>&1 | Out-String
  
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
