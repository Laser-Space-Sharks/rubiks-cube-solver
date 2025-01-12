#######################################################
####### Main cube Program!!! ##########################
#######################################################

# vauge stuff that needs to happen:
# wait for button press
# Move_to_faceN(0)
# fcolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, front, False))
# Move_to_face_N(1)
# ucolor = getCenterColor(captureImg(CUBE_IMG_FOLDER, up, False))
# colorsArray = genColorsArray(fcolor, ucolor)
# initialize cube Array
# cubeArr = addFaceToCubeArray(scanFace(colorsArray, True, "front"), orientation, cubeArr)
# addFaceToCubeArray(scanFace(colorsArray, True, "up"), orientation, cubeArr)
# for i=2; i++; i<6:
#   Move_to_faceN(i)
#   cubeArr = addFaceToCubeArray(scanCube(colorsArray))
#  shiftCubeArr = convertToShiftCube(cubeArr)
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

GPIO.setwarnings(False) # Ignore warnings from GPIO
GPIO.setmode(GPIO.BOARD) # Use physical pin numbering
GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) # Set pin 10 to be an input pin and set initial value to be pulled low (off)

while True:
    if GPIO.input(10) == GPIO.HIGH:
        print("Button was pushed!")
        #Start solving!
