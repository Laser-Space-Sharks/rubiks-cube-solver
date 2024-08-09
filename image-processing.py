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
import numpy as np
# import matplotlib as plt
from os import chdir
# from io import BytesIO
import subprocess

####### VARS #######
IMG_SIZE = 60 # for resolution
CUBE_IMG_FOLDER = "/home/pi/cubeImgs/"
# rows are face, cols are up
RIGHT_FACE_COLOR_ARRAY = [
    #[yellow, red, blue, white, orange, green]
    [-1, 2, 4, -1, 5, 1], #yellow is face
    [4, -1, 0, 2, -1, 3], #red is face
    [1, 3, -1, 4, 0, -1], #blue is face
    [-1, 5, 1, -1, 2, 4], #white is face
    [2, -1, 3, 4, -1, 0], #orange is face
    [4, 0, -1, 1, 3, -1] #green is face
]

####### SAVE AN IMAGE #######
def saveImg(image, directory, filename):
    chdir(directory)
    cv2.imwrite(filename, image)
    print(f"{filename} successfully saved!")

####### DELETE IMAGE FILE #######
def delImg(directory, filename):
    print(f"deleting {filename} from {directory}")
    chdir(directory)
    subprocess.run(["rm", f"{filename}.jpg"])

####### CAPTURE IMAGE WITH PICAMERA #######
# def captureImgPiCam():
#     camera = Picamera2()
#     camera.resolution = (imgSize, imgSize)
#     sleep(5) # give camera time to wake
#     image = BytesIO()
#     camera.capture_file(image, format='bmp')
#     return cv2.imdecode(image, cv2.IMREAD_COLOR)

####### CAPTURE IMAGE WITH LIBCAMERA #######
def captureImg(directory, filename):
    chdir(directory)
    # capture image using libcamera with specified resolution
    subprocess.run([
        "libcamera-still", 
        "-o", 
        f"{filename}.jpg", 
        "--width", 
        f"{IMG_SIZE}",
        "--height",
        f"{IMG_SIZE}"])
    
    # read image into cv2
    image = cv2.imread(f"{directory}{filename}.jpg", cv2.IMREAD_COLOR)
    # delete image from directory
    delImg(directory, filename)
    return image

####### NORMALIZE #######
def normalizeImg(image):
    # split into color channels
    r, g, b = cv2.split(image)

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
def colorAnalysis(pixel):
    # returns the color of the pixel as a number
    # yellow=0, red=1, blue=2, white=3, orange=4 green=5
    # to be implimented... sorry guys coming soon!
    print("hello world")

def genColorsArray(frontCenterColor, upCenterColor):
    # returns an array with elements representing cube faces
    # at an index that represents the color of that face
    # Elements up=0, right=1, front=2, left=3, back=4, down=5
    # Index [yellow, red, blue, white, orange, green]
    colorsArray = np.zeros(shape=(6))
    colorsArray[upCenterColor] = 0 # redundant but keep for now
    colorsArray[frontCenterColor] = 2
    rightCenterColor = RIGHT_FACE_COLOR_ARRAY[frontCenterColor][upCenterColor]
    colorsArray[rightCenterColor] = 1
    try:
        colorsArray[upCenterColor + 3] = 5
    except:
        colorsArray[upCenterColor - 3] = 5
    try:
        colorsArray[frontCenterColor + 3] = 4
    except:
        colorsArray[frontCenterColor - 3] = 4
    try:
        colorsArray[rightCenterColor + 3] = 3
    except:
        colorsArray[rightCenterColor - 3] = 3

    return colorsArray


####### Scan Cube #######
def scanFace(colorsArray):
    image = normalizeImg(captureImg(CUBE_IMG_FOLDER, "cubeScan"))
    imagePixels = [
        [image[IMG_SIZE/6][IMG_SIZE/6], image[IMG_SIZE/2][IMG_SIZE/6], image[5*(IMG_SIZE/6)][IMG_SIZE/6]],
        [image[IMG_SIZE/6][IMG_SIZE/2], image[IMG_SIZE/2][IMG_SIZE/2], image[5*(IMG_SIZE/6)][IMG_SIZE/2]],
        [image[IMG_SIZE/6][5*(IMG_SIZE/6)], image[IMG_SIZE/2][5*(IMG_SIZE/6)], image[5*(IMG_SIZE/6)][5*(IMG_SIZE/6)]]
    ]
    face = np.zeros(shape=(3,3))
    for i in range(3):
        for j in range(3):
            peice = imagePixels[i][j]
            face[i][j] = colorsArray[colorAnalysis(peice)]


def getCenterColor():
    image = normalizeImg(captureImg(CUBE_IMG_FOLDER, "cubeScan"))
    centerPix = image[IMG_SIZE/2][IMG_SIZE/2]
    return colorAnalysis(centerPix)

    

