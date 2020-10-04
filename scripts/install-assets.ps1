$ErrorActionPreference = "Stop"
$settings = (python ./scripts/settings.py | ConvertFrom-Json)

# install the module file
$moduleContent = (Get-Content $settings.moduleTemplatePath | Out-String) `
  -replace '{{modName}}',$settings.modName `
  -replace '{{modDescription}}',$settings.modDescription `
  -replace '{{modVersion}}',$settings.modVersion

[System.IO.File]::WriteAllLines($settings.moduleInstallPath, $moduleContent)
Write-Host $settings.moduleInstallPath "installed" -ForegroundColor "green"

# install mod sga file in the ic directory
Copy-Item $settings.emptySgaPath $settings.modSgaInstallPath -Force -Recurse
Write-Host $settings.modSgaInstallPath "installed" -ForegroundColor "green"

# install Locale/english/RDNMod/modloc.sga
New-Item "$($settings.modTextInstallPath)/.." -Force -Type Directory | Out-Null
Copy-Item $settings.emptySgaPath $settings.modlocInstallPath -Force
Write-Host $settings.modlocInstallPath "installed" -ForegroundColor "green"

# install the mod assets folder in the ic directory
Copy-Item $settings.dataFolder $settings.dataInstallFolder -Force -Recurse
Write-Host $settings.dataInstallFolder "installed" -ForegroundColor "green"
