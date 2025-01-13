#######################################################
####### Main cube Program!!! ##########################
#######################################################

# vauge stuff that needs to happen:


import RPi.GPIO as GPIO # Import Raspberry Pi GPIO library
from CubeScanProtocol import Move_to_faceN, Move_to_default
from servoCoding.DataTypes import Orientation
from image_processing import getCenterColor, CUBE_IMG_FOLDER,\
captureImg, genColorsArray, addFaceToCubeScan, convertToShiftCube, scanFace,\
errorDetection
from Command_Arduino import push_robotStrs_to_arduino
from subprocess import run 
from numpy import zeros 

GPIO.setwarnings(False) # Ignore warnings from GPIO
GPIO.setmode(GPIO.BOARD) # Use physical pin numbering
GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) # Set pin 10 to be an input pin and set initial value to be pulled low (off)

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
    # Scan rest of cube and convert to shift cube
    for i in range(2, 6):
        O: Orientation = Move_to_faceN(i)
        cubeArr = addFaceToCubeScan(scanFace(colorsArray), O, cubeArr)
    return cubeArr

while True:
    if GPIO.input(10) == GPIO.HIGH:
        print("Button was pushed!")
        # Start solving!
        cubeArr = scanCube()
        print(cubeArr)
        if (not errorDetection(cubeArr)):
            print("Cube scan failed!")
            Move_to_default()
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
            f"{shiftCubeArr[5]:x}"],
            check=True, stdout=PIPE).stdout
        push_robotStrs_to_arduino(solverOut.split())






