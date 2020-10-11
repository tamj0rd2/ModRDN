from settings import parseSettingsFile
from settings import Settings
import subprocess
import re
import time
import os
import argparse
from shutil import copyfile


def runProcess(args: list, throwOnFail=True):
    if (len(args) < 1):
        raise ValueError("you must provide a list of args")

    result = subprocess.run(args, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE, encoding="utf-8")

    if result.returncode != 0 and throwOnFail:
        raise ValueError("Received exit code {} when running command: {}".format(
            result.returncode, " ".join(args)))

    return result


def getVmState(settings: Settings):
    result = runProcess(["VBoxManage.exe", "showvminfo", settings.vmName])

    allOutput = [[line[0].strip(), line[1].strip()] for line in [re.sub(r"\s+", " ", rawLine).split(':', 1)
                                                                 for rawLine in result.stdout.split('\n') if rawLine != ''] if len(line) > 1]
    properties = dict(allOutput)
    return properties['State']


def resumeVm(settings: Settings):
    print("Resuming the VM...")
    runProcess(
        ["VBoxManage.exe", "controlvm", settings.vmName, "resume"])
    time.sleep(1)


def buildCppCode(settings: Settings):
    result = runProcess(["VBoxManage.exe", "guestcontrol", settings.vmName, "run",
                         "--", settings.guestBuildScriptLocation, settings.guestSolutionLocation], throwOnFail=False)

    if result.returncode == 0:
        print("cpp code built")
        return

    shouldStartPrinting = False
    stdout: str = result.stdout
    print(result.stderr)
    for line in stdout.strip().split("\n"):
        trimmedLine = line.strip()
        if re.match(r".*: (fatal )?(error|warning)", trimmedLine):
            shouldStartPrinting = True

        if shouldStartPrinting:
            if re.match(r".*: (fatal )?(error|warning)", trimmedLine):
                print(re.sub(r"\.(cpp|h)\((\d+)\)", ".\g<1>:\g<2>", trimmedLine))
            elif trimmedLine.lower().startswith(settings.guestCppProjectFolder.lower()):
                search = re.compile(r"{}\\(.*)\((\d+)\)".format(
                    re.escape(settings.guestCppProjectFolder)), re.IGNORECASE)
                print(re.sub(search, "\g<1>:\g<2>", trimmedLine))
            else:
                print(trimmedLine)
                continue


def buildLocale(settings: Settings):
    result = runProcess([settings.msBuildPath, settings.modTextSln,
                         "/p:configuration=release", "/property:GenerateFullPaths=true"])
    print("Locale built")


def startWindowsVM(settings: Settings):
    vmState = getVmState(settings)

    if ("powered off" in vmState):
        print("Starting the vm...")
        runProcess(["VBoxManage.exe", "startvm",
                    settings.vmName, "--type", "headless"])
        time.sleep(30)
        vmState = getVmState(settings)

    if ("paused" in vmState):
        resumeVm(settings)
        vmState = getVmState(settings)

    if ("running" in vmState):
        return

    raise ValueError("Could not start the {} vm. Current state: {}".format(
        settings.vmName, vmState))


def start(install: bool, launch: bool, targets: list):
    settings = parseSettingsFile()
    if "mod" in targets:
        startWindowsVM(settings)

    if "mod" in targets or "locale" in targets:
        print("Trying to build the code...")

        if "mod" in targets:
            buildCppCode(settings)
        if "locale" in targets:
            buildLocale(settings)

    if install or launch:
        runProcess(["taskkill", "/f", "/im", "IC.exe"], False)
        time.sleep(1)

        if "mod" in targets:
            copyfile(settings.dllOutputPath, settings.dllInstallPath)
            print("RDNMod.dll installed to {}".format(settings.dllInstallPath))

        if "locale" in targets:
            copyfile(settings.modTextDllOutputPath,
                     settings.modTextInstallPath)
            print("ModText.dll installed to {}".format(
                settings.modTextInstallPath))

        if "assets" in targets:
            print("warning: Asset installation not yet implemented in py. Using ps1")
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
