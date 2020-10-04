$ErrorActionPreference = "Stop"
$settings = (python ./scripts/settings.py | ConvertFrom-Json)

$icProcess = Get-Process ic -ErrorAction SilentlyContinue
if ($icProcess) {
  $icProcess | Stop-Process -Force
  Start-sleep 1
}

if (Test-Path $settings.dllInstallPath) {
  Remove-Item $settings.dllInstallPath
}
cp $settings.dllOutputPath $settings.dllInstallPath
Write-Host "RDNMod.dll installed to $($settings.dllInstallPath)" -ForegroundColor 'green'
