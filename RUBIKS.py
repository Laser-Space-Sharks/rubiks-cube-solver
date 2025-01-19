#######################################################
####### Main cube Program!!! ##########################
#######################################################

from RPi.GPIO import setwarnings, setmode, setup, input, HIGH, LOW, BOARD, PUD_DOWN, IN, OUT, output
from CubeScanProtocol import Move_to_faceN
from servoCoding.DataTypes import Orientation
from image_processing import getCenterColor, CUBE_IMG_FOLDER,\
captureImg, genColorsArray, addFaceToCubeScan, convertToShiftCube, scanFace,\
errorDetection, COLORS
from ServoController import execute, move_to_default
from subprocess import run, Popen, PIPE
from numpy import zeros, asarray
from time import sleep
from io import TextIOWrapper

setwarnings(False) # Ignore warnings from GPIO
setmode(BOARD) # Use physical pin numbering
setup(10, IN, pull_up_down=PUD_DOWN) # Set pin 10 to be an input pin and set initial value to be pulled low (off)
setup(11, OUT)
output(11, LOW)


def scanCube():
    # Start Scanning and initalize colors
    O1: Orientation = Move_to_faceN(0)
    ucolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, "up", False))
    O2: Orientation = Move_to_faceN(1)
    fcolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, "front", False))
    print(f"ucolor {COLORS[ucolor]} fcolor {COLORS[fcolor]}")
    colorsArray = genColorsArray(fcolor, ucolor)
    cubeArr = zeros(shape=(6, 3, 3))
    cubeArr = addFaceToCubeScan(scanFace(colorsArray, True, "front"), O2, cubeArr)
    cubeArr = addFaceToCubeScan(scanFace(colorsArray, True, "up"), O1, cubeArr)
    #check for error
    print(colorsArray)
    if cubeArr is None:
        return None
    # Scan rest of cube and convert to shift cube
    for i in range(2, 6):
        O: Orientation = Move_to_faceN(i)
        cubeArr = addFaceToCubeScan(scanFace(colorsArray), O, cubeArr)
        if cubeArr is None:
            return None
    return cubeArr

print("Ready To Go!")
while True:
    output(11, HIGH)
    if input(10) == HIGH:
        output(11, LOW)
        print("Button was pushed!")
        # Start solving!
        cubeArr = scanCube()
        # error checking 
        if cubeArr is None:
            print("Too dark to scan cube! Please try again with better lighting.")
            move_to_default()
            print("Ready To Go!")
            continue
        print(cubeArr)
        if (not errorDetection(cubeArr)):
            print("Cube scan failed!")
            move_to_default()
            print("Ready To Go!")
            continue
        shiftCubeArr = convertToShiftCube(cubeArr)
        print("Generated shift cube:")
        print(f"{shiftCubeArr[0]:x}")
        print(f"{shiftCubeArr[1]:x}")
        print(f"{shiftCubeArr[2]:x}")
        print(f"{shiftCubeArr[3]:x}")
        print(f"{shiftCubeArr[4]:x}")
        print(f"{shiftCubeArr[5]:x}")
        solverOut = Popen(
            "/home/pi/Documents/rubiks-cube-solver/solverc/shiftcube/solverpi " +
            "-i shiftcube -o servocode " +
            f"{shiftCubeArr[0]:x} " +
            f"{shiftCubeArr[1]:x} " +
            f"{shiftCubeArr[2]:x} " +
            f"{shiftCubeArr[3]:x} " +
            f"{shiftCubeArr[4]:x} " +
            f"{shiftCubeArr[5]:x} ",
            cwd="/home/pi/Documents/rubiks-cube-solver/solverc/shiftcube/",
            shell=True, close_fds=True, stdout=PIPE).stdout
        for i in TextIOWrapper(solverOut, encoding="utf-8"):
            print(f"Executing servocode: {i}")
            execute(i)
        move_to_default()
        print("Ready To Go!")
