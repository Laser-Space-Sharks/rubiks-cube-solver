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
import time
from picamera import PiCamera
import numpy as np
import matplotlib as plt
import os

####### SAVE AN IMAGE #######
def saveImg(image, directory, filename):
    os.chdir(directory)
    cv2.imwrite(filename, image)
    print(filename + " successfully saved!")

####### CAPTURE IMAGE #######
# vars
imgSize = 60 # for resolution

def captureImg():
    camera = Picamera()
    camera.resolution = (imgSize, imgSize)
    time.sleep(2) # give camera time to wake
    imageArray = np.empty(((imgSize^2) * 3), dtype=np.uint8)
    camera.capture(imageArray, 'bgr')
    return imageArray.reshape(imgSize, imgSize, 3)


####### NORMALIZE #######
def normalizeImg(image):
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
    image = normalizeImg(captureImg())
    saveImg(image, "/home/pi/", "test.jpg")

test()