from settings import parseSettingsFile
from settings import Settings
import subprocess
import re
import time
import os
import argparse
from shutil import copyfile
import sys
from zipfile import ZIP_DEFLATED, ZipFile


class Logger:
    GREEN = '\033[1;32m'
    RED = '\033[1;31m'
    BLUE = '\033[1;34m'
    YELLOW = '\033[93m'
    RESET = '\033[0m'

    @staticmethod
    def info(text: str):
        print(Logger.BLUE + text + Logger.RESET)

    @staticmethod
    def success(text: str):
        print(Logger.GREEN + text + Logger.RESET)

    @staticmethod
    def error(text: str):
        print(Logger.RED + text + Logger.RESET)

    @staticmethod
    def warn(text: str):
        print(Logger.YELLOW + text + Logger.RESET)

    @staticmethod
    def log(text: str):
        print(text)


def runProcess(args: list, throwOnFail=True):
    if (len(args) < 1):
        raise ValueError("you must provide a list of args")

    result = subprocess.run(args, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE, encoding="utf-8")

    if result.returncode != 0 and throwOnFail:
        raise ValueError("Received exit code {} when running command: {}".format(
            result.returncode, " ".join(args)))

    return result


class VmManager:
    def __init__(self, settings: Settings):
        self.vmName = settings.vmName

    def exec(self, args: list, throwOnFail=True, preExecMessage: str = None):
        self.__start()
        if preExecMessage:
            Logger.info(preExecMessage)

        result = runProcess(["VBoxManage.exe", "guestcontrol",
                             self.vmName, "run", "--"] + args, throwOnFail)
        self.__pause()
        return result

    def __start(self):
        vmState = self.__getState()

        if ("running" in vmState):
            return

        if ("paused" in vmState):
            runProcess(
                ["VBoxManage.exe", "controlvm", self.vmName, "resume"])
            return

        if ("powered off" in vmState or "aborted" in vmState):
            runProcess(["VBoxManage.exe", "startvm",
                        self.vmName, "--type", "headless"])
            time.sleep(1)
            vmState = self.__getState()

        self.__waitForAvailability()
        Logger.log("Connected to vm \"{}\"\n".format(self.vmName))
        return

    def __pause(self):
        runProcess(["VBoxManage.exe", "controlvm", self.vmName, "pause"])

    def __getState(self):
        result = runProcess(["VBoxManage.exe", "showvminfo", self.vmName])

        allOutput = [[line[0].strip(), line[1].strip()] for line in [re.sub(r"\s+", " ", rawLine).split(':', 1)
                                                                     for rawLine in result.stdout.split('\n') if rawLine != ''] if len(line) > 1]
        properties = dict(allOutput)
        return properties['State']

    def __waitForAvailability(self):
        Logger.info("Waiting for VM to become available")

        def isVmAvailable():
            result = runProcess(["VBoxManage.exe", "guestcontrol",
                                 self.vmName, "run", "--", "cmd.exe", "/C", "echo all good"], False)
            return result.returncode == 0

        attemptCount = 0
        while not isVmAvailable() or "running" not in self.__getState():
            Logger.log("waiting...")
            if (attemptCount > 6):
                raise ValueError("Could not start the {} vm. Current state: {}".format(
                    self.vmName, self.__getState()))
            attemptCount += 1
            time.sleep(5)


def buildCppCode(settings: Settings, vm: VmManager):
    result = vm.exec([settings.guestBuildScriptLocation,
                      settings.guestSolutionLocation], throwOnFail=False, preExecMessage="Building cpp code")
    if result.returncode == 0:
        Logger.success("Cpp code built\n")
        return

    shouldStartPrinting = False
    stdout: str = result.stdout

    for line in stdout.strip().split("\n"):
        trimmedLine = line.strip()
        if re.match(r".*: (fatal )?(error|warning)", trimmedLine):
            shouldStartPrinting = True

        def getLineWithReplacedPath():
            errorLineSearch = re.compile(r"(?:{})?(\\.*cpp|h)\((\d+)\)".format(
                re.escape(settings.guestCppProjectFolder)), re.IGNORECASE)
            lineWithReplacePaths = re.sub(
                errorLineSearch, "\g<1>:\g<2>", trimmedLine)

            if (lineWithReplacePaths.startswith('\\')):
                lineWithReplacePaths = lineWithReplacePaths[1:]

            return lineWithReplacePaths

        lineWithReplacedPaths = getLineWithReplacedPath()

        if shouldStartPrinting:
            if re.match(r".*: (fatal )?(error)", lineWithReplacedPaths):
                Logger.error(lineWithReplacedPaths)
            elif re.match(r".*: (warning)", lineWithReplacedPaths):
                Logger.warn(lineWithReplacedPaths)
            else:
                Logger.log(trimmedLine)
                continue

    raise ChildProcessError("Failed to build cpp code")


def buildLocale(settings: Settings):
    Logger.info("Building locale")
    runProcess([settings.msBuildPath, settings.modTextSln,
                "/p:configuration=release", "/property:GenerateFullPaths=true"])
    Logger.success("Locale built\n")


def getAllFilesInFolder(folderPath: str):
    files = []
    for folder in os.walk(folderPath):
        for fileName in folder[2]:
            files.append(os.path.join(folder[0], fileName))
    return files


def installFiles(settings: Settings, targets: list):
    runProcess(["taskkill", "/f", "/im", "IC.exe"], False)
    time.sleep(1)

    def installAsset(srcPath: str, installLocation: str):
        displayName = re.search(r"([^\\]+)$", srcPath)[0] or srcPath
        copyfile(srcPath, installLocation)
        Logger.info("{} installed to {}".format(displayName, installLocation))

    if "mod" in targets:
        installAsset(
            settings.dllOutputPath, settings.dllInstallPath)

    if "locale" in targets:
        installAsset(settings.modTextDllOutputPath,
                     settings.modTextInstallPath)

    if "assets" in targets:
        installAsset(settings.emptySgaPath, settings.modSgaInstallPath)
        installAsset(settings.emptySgaPath, settings.modlocInstallPath)

        with open(settings.moduleTemplatePath, "r") as templateFile:
            moduleContent = templateFile.read().replace("{{modName}}", settings.modName).replace(
                "{{modDescription}}", settings.modDescription).replace("{{modVersion}}", settings.modVersion)

            with open(settings.moduleInstallPath, "w") as moduleFile:
                moduleFile.write(moduleContent)
        Logger.info("template.module installed to {}".format(
            settings.moduleInstallPath))

        for installedDataFile in getAllFilesInFolder(settings.dataInstallFolder):
            if (installedDataFile.endswith(".lua")):
                os.remove(installedDataFile)

        for sourceDataFile in getAllFilesInFolder(settings.dataFolder):
            relativePath = sourceDataFile.split(settings.dataFolder)[1]
            installPath = "{}{}".format(settings.dataInstallFolder, relativePath)
            copyfile(sourceDataFile, installPath)

        Logger.info("{} installed to {}".format(settings.dataFolder, settings.dataInstallFolder))


def packFiles(settings: Settings):
    def writeFileToZip(zipf: ZipFile, filePath, installPath: str = None, zipName: str = None):
        installPath = installPath or filePath
        sanitizedZipLocation = re.sub(r"^(\.\.\\)+", "", os.path.relpath(installPath, settings.repoFolder))
        zipf.write(filePath, sanitizedZipLocation)

    if os.path.exists(settings.modExcludingDataZip):
        os.remove(settings.modExcludingDataZip)

    with ZipFile(settings.modExcludingDataZip, 'a', ZIP_DEFLATED) as zipfExcludingData:
        writeFileToZip(zipfExcludingData, settings.dllInstallPath)
        writeFileToZip(zipfExcludingData, settings.moduleInstallPath)
        writeFileToZip(zipfExcludingData, settings.emptySgaPath, installPath=settings.modSgaInstallPath)
        writeFileToZip(zipfExcludingData, settings.emptySgaPath, installPath=settings.modlocInstallPath)
        writeFileToZip(zipfExcludingData, settings.modTextInstallPath)
    Logger.log("Copied dll, metadata and locale files")

    copyfile(settings.modExcludingDataZip, settings.modIncludingDataZip)
    dataFiles = getAllFilesInFolder(settings.dataFolder)

    with ZipFile(settings.modIncludingDataZip, 'a', ZIP_DEFLATED) as zipfIncludingData:
        for dataFile in dataFiles:
            installPath = dataFile.replace("assets", settings.modName)
            writeFileToZip(zipfIncludingData, dataFile, installPath=installPath,
                           zipName="ModIncludingData.zip")
    Logger.log("Copied {} data files".format(len(dataFiles)))

    if os.path.exists(settings.luasOnlyZip):
        os.remove(settings.luasOnlyZip)

    with ZipFile(settings.luasOnlyZip, 'a', ZIP_DEFLATED) as zipfLuasOnly:
        for dataFile in dataFiles:
            if dataFile.endswith(".lua"):
                installPath = dataFile.replace("assets", settings.modName)
                writeFileToZip(zipfLuasOnly, dataFile, installPath=installPath,
                               zipName="LuasOnly.zip")

    Logger.success("Done packing zip files")


def start(install: bool, launch: bool, targets: list, pack: bool):
    settings = parseSettingsFile()
    vm = VmManager(settings)

    if "mod" in targets or "locale" in targets:
        if "mod" in targets:
            buildCppCode(settings, vm)
        if "locale" in targets:
            buildLocale(settings)

    if install or launch or pack:
        installFiles(settings, targets)

    if pack:
        packFiles(settings)

    if launch:
        subprocess.Popen([settings.icExePath, "-moddev"])


if __name__ == "__main__":
    try:
        parser = argparse.ArgumentParser()
        parser.add_argument("--install", action="store_true",
                            help="installs the DLL after the build")
        parser.add_argument("--launch", action="store_true",
                            help="launches IC after the build")
        parser.add_argument("--pack", action="store_true",
                            help="packages the files into a zip folder")
        possibleTargets = ["mod", "locale", "assets"]
        parser.add_argument("--targets", nargs="+",
                            default=possibleTargets, choices=possibleTargets)
        start(**parser.parse_args().__dict__)
    except Exception as err:
        print(err)
        Logger.error(getattr(err, 'message', repr(err)))
        sys.exit(1)
