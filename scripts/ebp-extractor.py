import os
import glob
import sys
import argparse
from settings import parseSettingsFile


class Result:
    def __init__(self, filePath: str, controllerType: int):
        self.filePath = filePath
        self.controllerType = 0 if controllerType > 100 else controllerType


def getEbpControllerType(filePath: str):
    with open(filePath, "rb") as f:
        last4Bytes = []

        while (byte := f.read(1)):
            if len(last4Bytes) < 4:
                last4Bytes.append(byte)
            else:
                last4Bytes.pop(0)
                last4Bytes.append(byte)

            if b"".join(last4Bytes) == b"EBPD":
                f.read(12)
                controllerType = int.from_bytes(f.read(4), "little")
                return Result(filePath, controllerType)


def filenameMatchesFilter(filename: str, matchStr: str):
    if matchStr is None:
        return True
    return matchStr in filename


def typeMatchesFilter(type: int, matchType: int, allowZero: bool):
    if type == 0:
        return allowZero or matchType == 0

    if matchType is not None:
        return type == matchType

    return True


def getEbpControllerTypeResults(searchDir: str, allowZero: bool, orderBy: str, matchFilename: str, matchType: str):
    os.chdir(searchDir)
    results = [getEbpControllerType(file)
               for file in glob.glob("**/*.ebp", recursive=True) if filenameMatchesFilter(file, matchFilename)]
    sortedResults = sorted(filter(None.__ne__, results), key=lambda result: result.controllerType if orderBy ==
                           "controllerType" else result.filePath)

    for result in sortedResults:
        if typeMatchesFilter(result.controllerType, matchType, allowZero):
            print("{}\t{}".format(result.controllerType, result.filePath))


def parseArgs():
    defaultSearchDir = parseSettingsFile().icInstallDirectory

    parser = argparse.ArgumentParser()
    parser.add_argument("--searchDir", type=str,
                        help="A folder to search for EBPs within", default=defaultSearchDir)
    parser.add_argument("--allowZero", action="store_true",
                        help="Show files whose controller type is 0")
    parser.add_argument(
        "--orderBy", choices=["controllerType", "fileName"], default="fileName", help="Choose ordering of results")
    parser.add_argument(
        "--matchFile", help="only show results if the filename matches the given string", dest="matchFilename")
    parser.add_argument("--matchType",
                        help="only show results if the controller type matches the given number", type=int)
    args = parser.parse_args()

    return args.__dict__


if __name__ == "__main__":
    getEbpControllerTypeResults(**parseArgs())
