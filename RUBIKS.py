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
# cubeArr = addFaceToCubeArray(scanFace(colorsArray, True, front), orientation, cubeArr)
# addFaceToCubeArray(scanFace(colorsArray, True, up), orientation, cubeArr)
# for i=2; i++; i<6:
#   Move_to_faceN(i)
#   addFaceToCubeArray(scanCube(colorsArray))