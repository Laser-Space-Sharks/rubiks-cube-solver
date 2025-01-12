#######################################################
####### Main cube Program!!! ##########################
#######################################################

# vauge stuff that needs to happen:
# run(["./solverc/shiftcube/solver", 
#    "-i", "shiftcube", 
#     "-o", "servocode", 
#     f"{shiftCubeArr[0]}", 
#     f"{shiftCubeArr[1]}", 
#     f"{shiftCubeArr[2]}", 
#     f"{shiftCubeArr[3]}", 
#     f"{shiftCubeArr[4]}", 
#     f"{shiftCubeArr[5]}"])

import RPi.GPIO as GPIO # Import Raspberry Pi GPIO library
from CubeScanProtocol import Move_to_faceN
from servoCoding.DataTypes import Orientation
from image_processing import getCenterColor, CUBE_IMG_FOLDER, captureImg, genColorsArray, addFaceToCubeScan, convertToShiftCube, scanFace
from subprocess import run 
from numpy import zeros 

GPIO.setwarnings(False) # Ignore warnings from GPIO
GPIO.setmode(GPIO.BOARD) # Use physical pin numbering
GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) # Set pin 10 to be an input pin and set initial value to be pulled low (off)

while True:
    if GPIO.input(10) == GPIO.HIGH:
        print("Button was pushed!")
        #Start solving!
        O1: Orientation = Move_to_faceN(0)
        ucolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, "up", false))
        O2: Orientation = Move_to_faceN(1)
        fcolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, "front", false))
        colorsArray = genColorsArray(fcolor, ucolor)
        cubeArr = zeros(shape=6)
        cubeArr = addFaceToCubeScan(scanFace(colorsArray, True, "front"), O2, cubeArr)
        cubeArr = addFaceToCubeScan(scanFace(colorsArray, True, "up"), O1, cubeArr)
        for i in range(2, 6):
            O: Orientation = Move_to_faceN(i)
            cubeArr = addFaceToCubeScan(scanFace(colorsArray))
        shiftCubeArr = convertToShiftCube(cubeArr)





