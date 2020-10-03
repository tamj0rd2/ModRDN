import os
import glob
import sys
import argparse


class Result:
    def __init__(self, filePath, controllerType):
        self.filePath = filePath
        self.controllerType = 0 if controllerType > 100 else controllerType


def getEbpControllerType(filePath):
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


def getEbpControllerTypeResults(searchDir, allowZero, orderBy):
    os.chdir(searchDir[0])

    results = [getEbpControllerType(file)
               for file in glob.glob("**/*.ebp", recursive=True)]
    sortedResults = sorted(filter(None.__ne__, results), key=lambda result: result.controllerType if orderBy ==
                           "controllerType" else result.filePath)

    for result in sortedResults:
        if (allowZero or result.controllerType > 0):
            print("{}\t{}".format(result.controllerType, result.filePath))


def parseArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("searchDir", nargs=1,
                        help="A folder to search for EBPs within")
    parser.add_argument("--allowZero", action="store_true",
                        help="Show files whose controller type is 0")
    parser.add_argument(
        "--orderBy", choices=["controllerType", "fileName"], default="fileName", help="Choose ordering of results")
    args = parser.parse_args()
    return args.__dict__


getEbpControllerTypeResults(**parseArgs())
