$settings = (python ./scripts/settings.py | ConvertFrom-Json)

Get-Content -Wait -Tail 60 "$($settings.icInstallDirectory)/warnings.log"