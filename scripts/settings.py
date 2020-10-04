import os
import sys
import json


class Settings:
    def __init__(self, repoFolder, vmName, guestMount, icInstallDirectory, modName):
        self.modName = modName
        self.vmName = vmName

        self.icInstallDirectory = icInstallDirectory
        self.icSdkDirectory = "{0}/SDK".format(icInstallDirectory)

        self.dllOutputPath = "{0}/{1}/Obj/bin/RDNMod.dll".format(
            self.icSdkDirectory, modName)
        self.dllInstallPath = "{0}/{1}.dll".format(icInstallDirectory, modName)

        localeProjectFolder = "{0}/locale-project".format(repoFolder)
        self.modTextSln = "{0}/Locale.sln".format(localeProjectFolder)
        self.modTextDllOutputPath = "{0}/Release/Locale.dll".format(
            localeProjectFolder)
        self.modTextInstallPath = "{0}/Locale/english/{1}/ModText.dll".format(
            icInstallDirectory, modName)

        guestProjectFolder = "{0}/{1}".format(guestMount, modName)
        self.guestCppProjectFolder = "{0}/cpp-project".format(
            guestProjectFolder)
        self.guestSolutionLocation = "{0}/RDNRelease.sln".format(
            self.guestCppProjectFolder)
        self.guestBuildScriptLocation = "{0}/scripts/build-guest-code.bat".format(
            guestProjectFolder)


def writeTemplateSettingsFile(filePath):
    template = """{
  "vmName": "windows xp",
  "modName": "RDNMod",
  "guestMount": "Z:",
  "icInstallDirectory": "D:/SteamLibrary/steamapps/common/Impossible Creatures_Dev"
}"""
    with open(filePath, "w") as f:
        f.write(template)


def parseSettingsFile():
    repoFolder = os.getcwd().replace("\\", "/")
    settingsFilePath = "{}/script-settings.json".format(repoFolder)

    if not os.path.isfile(settingsFilePath):
        writeTemplateSettingsFile(settingsFilePath)

    with open(settingsFilePath) as f:
        return Settings(repoFolder, **json.load(f)).__dict__


if __name__ == "__main__":
    print(parseSettingsFile())
