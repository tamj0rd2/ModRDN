from settings import parseSettingsFile
from settings import Settings
import subprocess
import re
import time
import os
import sys


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


def buildCode(settings: Settings):
    result = runProcess(["VBoxManage.exe", "guestcontrol", settings.vmName, "run",
                         "--", settings.guestBuildScriptLocation, settings.guestSolutionLocation], throwOnFail=False)

    if result.returncode == 0:
        print("Build succeded")
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


def startWindowsVM(settings: Settings):
    vmState = getVmState(settings)

    try:
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
            print("Trying to build the code...\n")
            return

        print("Could not start the vm in time")
        sys.exit(1)

        print("Could not start the vm. Got state {}".format(vmState))
    except:
        print('Problem starting the vm')
        sys.exit(1)


if __name__ == "__main__":
    settings = parseSettingsFile()
    startWindowsVM(settings)
    buildCode(settings)
