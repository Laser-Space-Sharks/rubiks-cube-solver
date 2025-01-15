#######################################################
####### Main cube Program!!! ##########################
#######################################################

from RPi.GPIO import setwarnings, setmode, setup, input, HIGH, BOARD, PUD_DOWN, IN
from CubeScanProtocol import Move_to_faceN
from servoCoding.DataTypes import Orientation
from image_processing import getCenterColor, CUBE_IMG_FOLDER,\
captureImg, genColorsArray, addFaceToCubeScan, convertToShiftCube, scanFace,\
errorDetection
from ServoController import execute, move_to_default
from subprocess import run 
from numpy import zeros

setwarnings(False) # Ignore warnings from GPIO
setmode(BOARD) # Use physical pin numbering
setup(10, IN, pull_up_down=PUD_DOWN) # Set pin 10 to be an input pin and set initial value to be pulled low (off)

print("Ready To Go!")

def scanCube():
    # Start Scanning and initalize colors
    O1: Orientation = Move_to_faceN(0)
    ucolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, "up", False))
    O2: Orientation = Move_to_faceN(1)
    fcolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, "front", False))
    colorsArray = genColorsArray(fcolor, ucolor)
    cubeArr = zeros(shape=(6, 3, 3))
    cubeArr = addFaceToCubeScan(scanFace(colorsArray, True, "front"), O2, cubeArr)
    cubeArr = addFaceToCubeScan(scanFace(colorsArray, True, "up"), O1, cubeArr)
    #check for error
    if cubeArr is None:
        return None
    # Scan rest of cube and convert to shift cube
    for i in range(2, 6):
        O: Orientation = Move_to_faceN(i)
        cubeArr = addFaceToCubeScan(scanFace(colorsArray), O, cubeArr)
        if cubeArr is None:
            return None
    return cubeArr

while True:
    if input(10) == HIGH:
        print("Button was pushed!")
        # Start solving!
        cubeArr = scanCube()
        # error checking 
        if cubeArr is None:
            print("Too dark to scan cube! Please try again with better lighting.")
            continue
        print(cubeArr)
        if (not errorDetection(cubeArr)):
            print("Cube scan failed!")
            move_to_default()
            continue
        shiftCubeArr = convertToShiftCube(cubeArr)
        # run solverc
        solverOut = run([
            "./solverc/shiftcube/solverpi", 
            "-i", "shiftcube", 
            "-o", "servocode", 
            f"{shiftCubeArr[0]:x}", 
            f"{shiftCubeArr[1]:x}",
            f"{shiftCubeArr[2]:x}", 
            f"{shiftCubeArr[3]:x}", 
            f"{shiftCubeArr[4]:x}", 
            f"{shiftCubeArr[5]:x}"
        ], check=True, stdout=PIPE).stdout
        for i in solverOut.strip().split(): execute(i)
        move_to_default()