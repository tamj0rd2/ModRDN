Write-Host "Trying to build ModText.dll..." -ForegroundColor 'blue'
& "D:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" .\Locale\Locale.sln /p:configurion=release /property:GenerateFullPaths=true

if (!$?) {
  exit 1
}
