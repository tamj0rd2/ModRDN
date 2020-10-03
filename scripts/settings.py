import os
import sys
import json


class Settings:
    def __init__(self, scriptsFolderPath, vmName, guestBuildScriptLocation, icInstallDirectory):
        self.vmName = vmName
        self.guestBuildScriptLocation = guestBuildScriptLocation
        self.icInstallDirectory = icInstallDirectory
        self.icSdkDirectory = icInstallDirectory + "/SDK"
        self.dllInstallPath = icInstallDirectory + "/RDNMod.dll"
        self.dllOutputPath = self.icSdkDirectory + "/Obj/bin/RDNMod.dll"
        self.modTextDllOutputPath = scriptsFolderPath + "/../Locale/Release/Locale.dll"
        self.modTextInstallPath = self.icInstallDirectory + \
            "/Locale/english/RDNMod/ModText.dll"


def writeTemplateSettingsFile(filePath):
    template = """{
  "vmName": "windows xp",
  "guestBuildScriptLocation": "Z:/ModRDNDevelopment/scripts/build-guest-code.bat",
  "icInstallDirectory": "D:/SteamLibrary/steamapps/common/Impossible Creatures_Dev",
}"""
    with open(filePath, "w") as f:
        f.write(template)


def parseSettingsFile():
    scriptsFolderPath = os.path.dirname(os.path.realpath(sys.argv[0]))
    settingsFilePath = scriptsFolderPath + '/settings.json'

    if not os.path.isfile(settingsFilePath):
        writeTemplateSettingsFile(settingsFilePath)

    with open(settingsFilePath) as f:
        return Settings(scriptsFolderPath, **json.load(f)).__dict__


if __name__ == "__main__":
    print(parseSettingsFile())
