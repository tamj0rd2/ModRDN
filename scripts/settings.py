import os
import sys
import json
from os import path


class Settings:
    def __init__(self, repoFolder: str, vmName: str, guestMount: str, icInstallDirectory: str, modName: str, modDescription: str, modVersion: str):
        self.repoFolder = repoFolder
        self.modExcludingDataZip = path.join(repoFolder, "ModExcludingData.zip")
        self.modIncludingDataZip = path.join(repoFolder, "ModIncludingData.zip")
        self.luasOnlyZip = path.join(repoFolder, "LuasOnly.zip")

        self.modName = modName
        self.modVersion = modVersion
        self.modDescription = modDescription

        self.icInstallDirectory = icInstallDirectory
        self.icExePath = "{}\\IC.exe".format(icInstallDirectory)
        self.icSdkDirectory = "{0}\\SDK".format(icInstallDirectory)

        self.dllOutputPath = "{0}\\{1}\\Obj\\bin\\RDNMod.dll".format(
            self.icSdkDirectory, modName)
        self.dllInstallPath = "{0}\\{1}.dll".format(
            icInstallDirectory, modName)

        localeProjectFolder = "{0}\\locale-project".format(repoFolder)
        self.modTextSln = "{0}\\Locale.sln".format(localeProjectFolder)
        self.modTextDllOutputPath = "{0}\\Release\\Locale.dll".format(
            localeProjectFolder)
        localeInstallFolder = "{0}\\Locale\\english\\{1}".format(
            icInstallDirectory, modName)
        self.modTextInstallPath = "{0}\\ModText.dll".format(
            localeInstallFolder)
        self.modlocInstallPath = "{0}\\modloc.sga".format(localeInstallFolder)
        self.msBuildPath = "D:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe"

        self.vmName = vmName
        guestProjectFolder = "{0}\\{1}".format(guestMount, modName)
        self.guestCppProjectFolder = "{0}\\cpp-project".format(
            guestProjectFolder)
        self.guestSolutionLocation = "{0}\\RDNRelease.sln".format(
            self.guestCppProjectFolder)
        self.guestBuildScriptLocation = "{0}\\scripts\\build-guest-code.bat".format(
            guestProjectFolder)

        self.assetsFolder = "{}\\assets".format(repoFolder)
        self.moduleTemplatePath = "{}\\template.module".format(self.assetsFolder)
        self.moduleInstallPath = "{}\\{}.module".format(
            icInstallDirectory, modName.lower())
        self.emptySgaPath = "{}\\empty.sga".format(self.assetsFolder)
        self.dataFolder = "{}\\Data".format(self.assetsFolder)
        self.dataInstallFolder = "{}\\{}\\Data".format(
            icInstallDirectory, modName)
        self.modSgaInstallPath = "{}\\{}Data.sga".format(
            icInstallDirectory, modName)


def writeTemplateSettingsFile(filePath):
    template = """{
  "vmName": "windows xp",
  "modName": "RDNMod",
  "modDescription": "Baby's first mod",
  "modVersion": "0.0.1",
  "guestMount": "Z:",
  "icInstallDirectory": "D:\\SteamLibrary\\steamapps\\common\\Impossible Creatures_Dev"
}"""
    with open(filePath, "w") as f:
        f.write(template)


def parseSettingsFile():
    repoFolder = os.getcwd().replace("\\", "/")
    settingsFilePath = "{}\\script-settings.json".format(repoFolder)

    if not os.path.isfile(settingsFilePath):
        writeTemplateSettingsFile(settingsFilePath)

    with open(settingsFilePath) as f:
        return Settings(repoFolder, **json.load(f))


if __name__ == "__main__":
    print(parseSettingsFile().__dict__)
