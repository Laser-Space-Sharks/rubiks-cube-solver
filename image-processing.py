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

# capture and normalize Image
# image = normalizeImg(captureImg(CUBE_IMG_FOLDER, "cubeScan"))

####### IMPORTS #######
import cv2
from time import sleep, perf_counter
# from picamera2 import Picamera2
from numpy import asarray, zeros
# import matplotlib as plt
from os import chdir
# from io import BytesIO
import subprocess

####### VARS #######
IMG_SIZE = 60 # for resolution
PIECE_SIZE = IMG_SIZE//3
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

LOWER_BOUND_COLORS = [
    asarray([41, 60, 30]), # yellow
    asarray([350, 60, 30]), # red
    asarray([146, 60, 30]), # blue
    asarray([0, 0, 50]), # white
    asarray([21, 60, 30]), # orange
    asarray([71, 60, 30]), # green
]

UPPER_BOUND_COLORS = [
    asarray([70, 100, 100]), # yellow
    asarray([20, 100, 100]), # red
    asarray([260, 100, 100]), # blue
    asarray([100, 100, 100]), # white
    asarray([40, 100, 100]), # orange
    asarray([260, 100, 100]), # green
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

# This one takes less time on our raspberrypi 1
####### CAPTURE IMAGE WITH LIBCAMERA #######
def captureImg(directory, filename, delete=True):
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
    if delete:
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
def colorAnalysis(peice):
    # returns the color of the peice as a number
    # yellow=0, red=1, blue=2, white=3, orange=4 green=5
    for i in range(6):
        # mask all colors except the color we are looking for right now
        # turns into an array where everything that is a 1 is in that range, everything else is 0
        mask = cv2.inRange(peice, LOWER_BOUND_COLORS[i], UPPER_BOUND_COLORS[i])
        print(f"color is {i} \n {mask}")
        # find the percentage of the piece that is in that color range
        percent = (cv2.countNonZero(mask)//(PIECE_SIZE^2)) * 100
        # if more than 70% of the cube is that color, return the color
        if percent >= 70:
            return i
    # if nothing hits you've got a problem 
    return -1

def genColorsArray(frontCenterColor, upCenterColor):
    # returns an array with elements representing cube faces
    # at an index that represents the color of that face
    # Elements up=0, right=1, front=2, left=3, back=4, down=5
    # Index [yellow, red, blue, white, orange, green]
    colorsArray = zeros(shape=(6)) # create array of zeros 
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
def scanFace(image, colorsArray):
    # split image into 9 sections
    imagePixels = [
        # First row of peices shiftcube->(1, 2, 3)
        [
            image[0:IMG_SIZE//3, 0:IMG_SIZE//3], 
            image[0:IMG_SIZE//3, IMG_SIZE//3:2*(IMG_SIZE)//3], 
            image[0:IMG_SIZE//3, 2*(IMG_SIZE)//3:IMG_SIZE]
        ],
        # second row of peices shiftcube->(8, center, 4)
        [
            image[IMG_SIZE//3:2*(IMG_SIZE)//3, 0:IMG_SIZE//3], 
            image[IMG_SIZE//3:2*(IMG_SIZE)//3, IMG_SIZE//3:2*(IMG_SIZE)//3], 
            image[IMG_SIZE//3:2*(IMG_SIZE)//3, 2*(IMG_SIZE)//3:IMG_SIZE]
        ],
        # third row of peices shiftcube->(7, 6, 5)
        [
            image[2*(IMG_SIZE)//3:IMG_SIZE, 0:IMG_SIZE//3], 
            image[2*(IMG_SIZE)//3:IMG_SIZE, IMG_SIZE//3:2*(IMG_SIZE)//3], 
            image[2*(IMG_SIZE)//3:IMG_SIZE, 2*(IMG_SIZE)//3:IMG_SIZE]
        ]
    ]
    # represent face as array 
    face = zeros(shape=(3,3))
    for i in range(3):
        for j in range(3):
            peice = imagePixels[i][j]
            face[i][j] = colorsArray[colorAnalysis(peice)]

    return face


def getCenterColor(image):
    centerPiece = image[IMG_SIZE//3:2*(IMG_SIZE)//3, IMG_SIZE//3:2*(IMG_SIZE)//3]
    return colorAnalysis(centerPiece)
    

####### FORMAT TO SHIFTCUBE #######
def convertToShiftCube(cubeArray):
    shiftCube = zeros(shape=6, dtype=uint64)
    return shiftCube

####### TESTING #######
def test():
    print("Starting Test!")
    image = captureImg("/home/pi/cubeImgs/", "testCube", delete=False)
    print("image captured!")
    image = normalizeImg(image)
    colorsArray = genColorsArray(2, 1)
    print("starting face scan!")
    cubeArray = scanFace(image, colorsArray)
    print("face scanned!")
    print(cubeArray)

def testImgNormal():
    print("Starting Test!")
    image = captureImg("/home/pi/cubeImgs/", "testCube", delete=False)
    print("image captured!")
    normImg = normalizeImg(image)
    saveImg(normImg, "/home/pi/cubeImgs/", "testCubeNorm.jpg")

test()
