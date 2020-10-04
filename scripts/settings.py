import os
import sys
import json


class Settings:
    def __init__(self, repoFolder, vmName, guestMount, icInstallDirectory, modName):
        self.modName = modName
        self.vmName = vmName

        self.icInstallDirectory = icInstallDirectory
        self.icSdkDirectory = "{0}/SDK".format(icInstallDirectory)

        self.dllOutputPath = "{0}/Obj/bin/{1}.dll".format(
            self.icSdkDirectory, modName)
        self.dllInstallPath = "{0}/{1}.dll".format(icInstallDirectory, modName)

        localeProjectFolder = "{0}/locale-project".format(repoFolder)
        self.modTextSln = "{0}/Locale.sln".format(localeProjectFolder)
        self.modTextDllOutputPath = "{0}/Release/Locale.dll".format(
            localeProjectFolder)
        self.modTextInstallPath = "{0}/Locale/english/{1}/ModText.dll".format(
            icInstallDirectory, modName)

        self.guestProjectFolder = "{0}/{1}".format(guestMount, modName)
        self.guestBuildScriptLocation = "{0}/scripts/build-guest-code.bat".format(
            self.guestProjectFolder)
        self.guestSolutionLocation = "{0}/RDNRelease.sln".format(
            self.guestProjectFolder)


def writeTemplateSettingsFile(filePath):
    template = """{
  "vmName": "windows xp",
  "modName": "TMod",
  "guestMount": "Z:",
  "icInstallDirectory": "D:/SteamLibrary/steamapps/common/Impossible Creatures_Dev"
}"""
    with open(filePath, "w") as f:
        f.write(template)


def parseSettingsFile():
    repoFolder = os.getcwd().replace("\\", "/")
    settingsFilePath = "{}/scripts/settings.json".format(repoFolder)

    if not os.path.isfile(settingsFilePath):
        writeTemplateSettingsFile(settingsFilePath)

    with open(settingsFilePath) as f:
        return Settings(repoFolder, **json.load(f)).__dict__


if __name__ == "__main__":
    print(parseSettingsFile())
