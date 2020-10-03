$ErrorActionPreference = "Stop"
$settings = (python ./scripts/settings.py | ConvertFrom-Json)

New-Item "$($settings.modTextInstallPath)/.." -Force -Type Directory
if (Test-Path $settings.modTextInstallPath) {
  Remove-Item $settings.modTextInstallPath
}
Copy-Item $settings.modTextDllOutputPath $settings.modTextInstallPath
Write-Host "ModText.dll installed to $($settings.modTextInstallPath)" -ForegroundColor 'green'
