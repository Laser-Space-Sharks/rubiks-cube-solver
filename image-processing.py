# Things that need to happen here
# in not a lot of detail and probably wrong
# libcamera-still -o test.jpg --width 60 --height 60
# 1. Normalize
# 2. Split image into 9 sections representing the peices
# 3. Cycle through the peices centers 
# 4. identify the color usng cv2.inRange 
# 5. correct centers for white/logo error 
# using opposing center rules
# Then store the cube

####### IMPORTS #######
import cv2
from time import sleep, perf_counter
# from picamera2 import Picamera2
# import numpy as np
# import matplotlib as plt
from os import chdir
# from io import BytesIO
import subprocess

####### SAVE AN IMAGE #######
def saveImg(image, directory, filename):
    chdir(directory)
    cv2.imwrite(filename, image)
    print(f"{filename} successfully saved!")

####### DELETE IMAGE FILE #######
def delImg(directory, filename):
    print(f"deleting {filename} from {directory}")
    chdir(directory)
    subprocess.run(["rm", f"{filename}"])

####### CAPTURE IMAGE WITH PICAMERA #######
# vars
imgSize = 60 # for resolution

# def captureImgPiCam():
#     camera = Picamera2()
#     camera.resolution = (imgSize, imgSize)
#     sleep(5) # give camera time to wake
#     image = BytesIO()
#     camera.capture_file(image, format='bmp')
#     return cv2.imdecode(image, cv2.IMREAD_COLOR)

####### CAPTURE IMAGE WITH LIBCAMERA #######
def captureImg(directory, filename):
    # capture image using libcamera with specified resolution
    subprocess.run([
        "libcamera-still", 
        "-o", 
        f"{filename}.jpg", 
        "--width", 
        f"{imgSize}",
        "--height",
        f"{imgSize}"])
    
    # read image into cv2
    image = cv2.imread(f"{directory}{filename}.jpg")
    # delete image from directory
    delImg(directory, filename)
    return image

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
def test(directory):
    image = normalizeImg(captureImg(directory, "test"))
    saveImg(image, directory, "normal.jpg")

test("/home/pi/cubeImgs/")