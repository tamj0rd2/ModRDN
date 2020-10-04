$ErrorActionPreference = "Stop"
$settings = (python ./scripts/settings.py | ConvertFrom-Json)

$moduleContent = (Get-Content $settings.moduleTemplatePath | Out-String) `
  -replace '{{modName}}',$settings.modName `
  -replace '{{modDescription}}',$settings.modDescription `
  -replace '{{modVersion}}',$settings.modVersion

[System.IO.File]::WriteAllLines($settings.moduleInstallPath, $moduleContent)
Write-Host $settings.moduleInstallPath "installed" -ForegroundColor "green"
