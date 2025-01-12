#######################################################
####### Main cube Program!!! ##########################
#######################################################

# vauge stuff that needs to happen:


import RPi.GPIO as GPIO # Import Raspberry Pi GPIO library
from CubeScanProtocol import Move_to_faceN
from servoCoding.DataTypes import Orientation
from image_processing import getCenterColor, CUBE_IMG_FOLDER, captureImg, genColorsArray, addFaceToCubeScan, convertToShiftCube, scanFace
from subprocess import run 
from numpy import zeros 

GPIO.setwarnings(False) # Ignore warnings from GPIO
GPIO.setmode(GPIO.BOARD) # Use physical pin numbering
GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) # Set pin 10 to be an input pin and set initial value to be pulled low (off)

print("Ready To Go!")

while True:
    if GPIO.input(10) == GPIO.HIGH:
        print("Button was pushed!")
        # Start solving!
        # Start Scanning and initalize colors
        O1: Orientation = Move_to_faceN(0)
        ucolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, "up", False))
        O2: Orientation = Move_to_faceN(1)
        fcolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, "front", False))
        colorsArray = genColorsArray(fcolor, ucolor)
        cubeArr = zeros(shape=6, 3, 3)
        cubeArr = addFaceToCubeScan(scanFace(colorsArray, True, "front"), O2, cubeArr)
        cubeArr = addFaceToCubeScan(scanFace(colorsArray, True, "up"), O1, cubeArr)
        # Scan rest of cube and convert to shift cube
        for i in range(2, 6):
            O: Orientation = Move_to_faceN(i)
            cubeArr = addFaceToCubeScan(scanFace(colorsArray), O, cubeArr)
        shiftCubeArr = convertToShiftCube(cubeArr)
        # run solverc
        run([
            "./solverc/shiftcube/solver", 
            "-i", "shiftcube", 
            "-o", "servocode", 
            f"{shiftCubeArr[0]}", 
            f"{shiftCubeArr[1]}",
            f"{shiftCubeArr[2]}", 
            f"{shiftCubeArr[3]}", 
            f"{shiftCubeArr[4]}", 
            f"{shiftCubeArr[5]}"])






