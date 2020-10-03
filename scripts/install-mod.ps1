$ErrorActionPreference = "Stop"
$settings = (python ./scripts/settings.py | ConvertFrom-Json)

if (Test-Path $settings.dllInstallPath) {
  Remove-Item $settings.dllInstallPath
}
cp $settings.dllOutputPath $settings.dllInstallPath
Write-Host "RDNMod.dll installed to $($settings.dllInstallPath)" -ForegroundColor 'green'
