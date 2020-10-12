from settings import parseSettingsFile
from settings import Settings
import subprocess
import re
import time
import os
import argparse
from shutil import copyfile


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
        print("Connected to vm \"{}\"".format(self.vmName))
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
            print("waiting...")
            if (attemptCount > 30):
                raise ValueError("Could not start the {} vm. Current state: {}".format(
                    self.vmName, self.__getState()))
            attemptCount += 1
            time.sleep(1)


def buildCppCode(settings: Settings, vm: VmManager):
    result = vm.exec([settings.guestBuildScriptLocation,
                      settings.guestSolutionLocation], throwOnFail=True, preExecMessage="Building cpp code")
    if result.returncode == 0:
        Logger.success("Cpp code built\n")
        return

    shouldStartPrinting = False
    stdout: str = result.stdout

    for line in stdout.strip().split("\n"):
        trimmedLine = line.strip()
        if re.match(r".*: (fatal )?(error|warning)", trimmedLine):
            shouldStartPrinting = True

        if shouldStartPrinting:
            if re.match(r".*: (fatal )?(error|warning)", trimmedLine):
                Logger.error(re.sub(r"\.(cpp|h)\((\d+)\)",
                                    ".\g<1>:\g<2>", trimmedLine))
            elif trimmedLine.lower().startswith(settings.guestCppProjectFolder.lower()):
                search = re.compile(r"{}\\(.*)\((\d+)\)".format(
                    re.escape(settings.guestCppProjectFolder)), re.IGNORECASE)
                Logger.error(re.sub(search, "\g<1>:\g<2>", trimmedLine))
            else:
                Logger.error(trimmedLine)
                continue


def buildLocale(settings: Settings):
    Logger.info("Building locale")
    runProcess([settings.msBuildPath, settings.modTextSln,
                "/p:configuration=release", "/property:GenerateFullPaths=true"])
    Logger.success("Locale built\n")


def start(install: bool, launch: bool, targets: list):
    settings = parseSettingsFile()
    vm = VmManager(settings)

    if "mod" in targets or "locale" in targets:
        if "mod" in targets:
            buildCppCode(settings, vm)
        if "locale" in targets:
            buildLocale(settings)

    if install or launch:
        runProcess(["taskkill", "/f", "/im", "IC.exe"], False)
        time.sleep(1)

        if "mod" in targets:
            copyfile(settings.dllOutputPath, settings.dllInstallPath)
            Logger.success("RDNMod.dll installed to {}".format(
                settings.dllInstallPath))

        if "locale" in targets:
            copyfile(settings.modTextDllOutputPath,
                     settings.modTextInstallPath)
            Logger.success("ModText.dll installed to {}".format(
                settings.modTextInstallPath))

        if "assets" in targets:
            Logger.warn(
                "warning: Asset installation not yet implemented in py. Using ps1")
            result = runProcess(
                ["powershell", "-file", '{}\\scripts\\install-assets.ps1'.format(settings.repoFolder).replace("/", "\\")])
            print(result.stdout)

    if launch:
        subprocess.Popen([settings.icExePath, "-moddev"])


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--install", action="store_true",
                        help="installs the DLL after the build")
    parser.add_argument("--launch", action="store_true",
                        help="launches IC after the build")
    possibleTargets = ["mod", "locale", "assets"]
    parser.add_argument("--targets", nargs="+",
                        default=possibleTargets, choices=possibleTargets)
    start(**parser.parse_args().__dict__)
