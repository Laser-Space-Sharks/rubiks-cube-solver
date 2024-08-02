# Things that need to happen here
# in not a lot of detail and probably wrong
# 1. Normalize
# 2. Find cube face using edge detection 
# and it's cubeSize
# 3. Cycle through the peices centers using 
# info from cubeSize and take its color
# 4. identify the color usng cv.inRange 
# 5. correct centers for white/logo error 
# using opposing center rules
# 6. store cube state 

####### IMPORTS #######
import cv2
from time import sleep, time
from picamera2 import Picamera2
# import numpy as np
# import matplotlib as plt
from os import chdir
from io import BytesIO

startTime = time()

####### SAVE AN IMAGE #######
def saveImg(image, directory, filename):
    print("saving an image at " + str((time() - startTime)))
    chdir(directory)
    cv2.imwrite(filename, image)
    print(filename + " successfully saved!")

####### CAPTURE IMAGE #######
# vars
imgSize = 60 # for resolution

def captureImg():
    print("start img capture at " + str((time() - startTime)))
    camera = Picamera2()
    camera.resolution = (imgSize, imgSize)
    sleep(5) # give camera time to wake
    image = BytesIO()
    print("about to hit the long bit :( at " + str((time() - startTime)))
    camera.capture_file(image, format='bmp')
    print("got past the long bit!! at " + str((time() - startTime)))
    return cv2.imdecode(image, cv2.IMREAD_COLOR)


####### NORMALIZE #######
def normalizeImg(image):
    print("statring normalization at " + str((time() - startTime)))
    # split into color channels
    b, g, r = cv2.split(image)

    # normalize channels
    bNorm = cv2.normalize(b, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX)
    gNorm = cv2.normalize(g, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX)
    rNorm = cv2.normalize(r, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX)
    
    # merge
    mergedImage = cv2.merge([rNorm, gNorm, bNorm])

    # changing colorspaces
    normalizedImage = cv2.cvtColor(mergedImage, cv2.COLOR_RGB2HSV)
    # Here we are using hue saturation value (HSV) in order to easily define 
    # a range of values that we can the colors of a given cube to fall between

    return normalizedImage

####### Color Analysis #######

####### Testing #######
def test():
    print("starting the test at " + str((time() - startTime)))
    image = normalizeImg(captureImg())
    saveImg(image, "/home/pi/", "test.jpg")

test()