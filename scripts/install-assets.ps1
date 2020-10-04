$ErrorActionPreference = "Stop"
$settings = (python ./scripts/settings.py | ConvertFrom-Json)

# install the module file
$moduleContent = (Get-Content $settings.moduleTemplatePath | Out-String) `
  -replace '{{modName}}',$settings.modName `
  -replace '{{modDescription}}',$settings.modDescription `
  -replace '{{modVersion}}',$settings.modVersion

[System.IO.File]::WriteAllLines($settings.moduleInstallPath, $moduleContent)
Write-Host $settings.moduleInstallPath "installed" -ForegroundColor "green"

# install Locale/english/RDNMod/modloc.sga
New-Item "$($settings.modTextInstallPath)/.." -Force -Type Directory
Copy-Item $settings.emptySgaPath $settings.modlocInstallPath -Force
Write-Host $settings.modlocInstallPath "installed" -ForegroundColor "green"
