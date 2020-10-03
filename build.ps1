$ErrorActionPreference = "Stop"
$settings = (python ./scripts/settings.py | ConvertFrom-Json)
write-output $settings

# builds mod files
./scripts/build-mod.ps1
./scripts/build-locale.ps1

# installs them into the iC directory
./scripts/install-mod.ps1
./scripts/install-locale.ps1
