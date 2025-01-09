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
from cv2 import imwrite, imread, IMREAD_COLOR, split, normalize,\
NORM_MINMAX, cvtColor, COLOR_BGR2HSV, COLOR_HSV2BGR, merge, inRange,\
imshow, countNonZero, waitKey, bitwise_or, imdecode
# from picamera2 import Picamera2
from numpy import asarray, zeros, where, copy, uint32
# import matplotlib as plt
from os import chdir
# from io import BytesIO
from subprocess import run

#######################################################
####### Constant Variables            #################
####### (Because hard coding is bad!) #################
#######################################################

Abbie_img_folder = "/home/aagowl/Downloads/"
IMG_WIDTH = 56 # for resolution
IMG_HEIGHT = 64
PIECE_SIZE = (IMG_WIDTH//3) * (IMG_HEIGHT//3)
CUBE_IMG_FOLDER = "/home/pi/cubeImgs/"
# colors array good for translating numerical colors into english
# does not work when cube translated into colorblind notation (obvi)
# use translateToColors() with a colorsArray to translate colorblind notation
COLORS = ["yellow", "red", "blue", "white", "orange", "green"]
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

# determines what colors are defined as in HSV
LOWER_BOUND_COLORS = [
    asarray([25, 50, 70]), # yellow
    asarray([159, 50, 70]), # red
    asarray([90, 50, 70]), # blue
    asarray([0, 0, 170]), # white
    asarray([7, 50, 70]), # orange
    asarray([36, 50, 70]), # green
]

UPPER_BOUND_COLORS = [
    asarray([52, 255, 255]), # yellow
    asarray([180, 255, 255]), # red
    asarray([128, 255, 255]), # blue
    asarray([190, 47, 255]), # white
    asarray([24, 255, 255]), # orange
    asarray([89, 255, 255]), # green
]

#######################################################
####### File Actions ##################################
#######################################################

####### SAVE AN IMAGE #######
def saveImg(image, directory, filename):
    chdir(directory)
    imwrite(filename, image)
    print(f"{filename} successfully saved!")

####### DELETE IMAGE FILE #######
def delImg(directory, filename):
    print(f"deleting {filename} from {directory}")
    chdir(directory)
    run(["rm", f"{filename}.jpg"])

# caution untested
# this did not work with our device but it is
# the "proper" way to capture an image
####### CAPTURE IMAGE WITH PICAMERA #######
# def captureImgPiCam():
#     camera = Picamera2()
#     camera.resolution = (imgSize, imgSize)
#     sleep(5) # give camera time to wake
#     image = BytesIO()
#     camera.capture_file(image, format='bmp')
#     return imdecode(image, IMREAD_COLOR)

# This one takes less time on our raspberrypi 1
####### CAPTURE IMAGE WITH LIBCAMERA #######
def captureImg(directory, filename, delete=True):
    chdir(directory)
    # capture image using libcamera with specified resolution
    run([
        "libcamera-still", 
        "-o", 
        f"{filename}.jpg", 
        "--width", 
        f"{IMG_WIDTH}",
        "--height",
        f"{IMG_HEIGHT}"])
    
    # read image into cv2, which uses bgr by default
    image = imread(f"{directory}{filename}.jpg", IMREAD_COLOR)
    # delete image from directory
    if delete:
        delImg(directory, filename)
    return image

#######################################################
####### Color Analysis ################################
#######################################################

def colorAnalysis(peice):
    # returns the color of the peice as a number
    # yellow=0, red=1, blue=2, white=3, orange=4 green=5
    for i in range(6):
        # mask all colors except the color we are looking for right now
        # turns into an array where everything that is set to 255 is in that range, everything else is 0
        peiceCopy = peice
        mask = inRange(peiceCopy, LOWER_BOUND_COLORS[i], UPPER_BOUND_COLORS[i])
        # print(f"checking for {COLORS[i]}")
        # imshow('Mask', mask) # CHANGE
        # waitKey(0) # CHANGE
        # find the percentage of the piece that is in that color range
        percent = (countNonZero(mask)/(PIECE_SIZE)) * 100
        # if more than 55% of the peice is that color, return the color
        if percent >= 55 or (i == 1 and redColorCheck(peiceCopy, mask) >= 55):
            return i
    # if nothing hits its probably white
    return 3

def redColorCheck(peiceCopy, mask1):
    red2Upper = asarray([6, 255, 255])
    red2Lower = asarray([0, 50, 70])
    mask2 = inRange(peiceCopy, red2Lower, red2Upper)
    result = bitwise_or(mask1, mask2)
    return (countNonZero(result)/(PIECE_SIZE)) * 100

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
    # Here ifs are used to eliminate out of bounds errors.
    # Exploits fact that the same colors are always opposite of eachother
    #down
    if upCenterColor + 3 < 5:
        colorsArray[upCenterColor + 3] = 5
    else:
        colorsArray[upCenterColor - 3] = 5
    #back
    if frontCenterColor + 3 < 5:
        colorsArray[frontCenterColor + 3] = 4
    else:
        colorsArray[frontCenterColor - 3] = 4
    #left
    if rightCenterColor + 3 < 5:
        colorsArray[rightCenterColor + 3] = 3
    else:
        colorsArray[rightCenterColor - 3] = 3

    return colorsArray

def translateToColors(faceArray, colorsArray):
    faceColors = [["", "", ""],["", "", ""],["", "", ""]]
    for i in range(3):
        for j in range(3):
            faceColors[i][j] = COLORS[int(where(colorsArray == faceArray[i][j].item())[0].item())]
    
    return faceColors

#######################################################
####### Scan Cube #####################################
#######################################################

def analyzeFace(image, colorsArray):
    # split image into 9 sections
    imagePixels = [
        # First row of peices shiftcube->(1, 2, 3)
        [
            image[0:IMG_HEIGHT//3, 0:IMG_WIDTH//3], 
            image[0:IMG_HEIGHT//3, IMG_WIDTH//3:2*(IMG_WIDTH)//3], 
            image[0:IMG_HEIGHT//3, 2*(IMG_WIDTH)//3:IMG_WIDTH]
        ],
        # second row of peices shiftcube->(8, center, 4)
        [
            image[IMG_HEIGHT//3:2*(IMG_HEIGHT)//3, 0:IMG_WIDTH//3], 
            image[IMG_HEIGHT//3:2*(IMG_HEIGHT)//3, IMG_WIDTH//3:2*(IMG_WIDTH)//3], 
            image[IMG_HEIGHT//3:2*(IMG_HEIGHT)//3, 2*(IMG_WIDTH)//3:IMG_WIDTH]
        ],
        # third row of peices shiftcube->(7, 6, 5)
        [
            image[2*(IMG_HEIGHT)//3:IMG_HEIGHT, 0:IMG_WIDTH//3], 
            image[2*(IMG_HEIGHT)//3:IMG_HEIGHT, IMG_WIDTH//3:2*(IMG_WIDTH)//3], 
            image[2*(IMG_HEIGHT)//3:IMG_HEIGHT, 2*(IMG_WIDTH)//3:IMG_WIDTH]
        ]
    ]
    peiceNamesRow = ["upper", "center", "lower"]
    peiceNamesCol = ["left", "center", "right"]
    # represent face as array 
    face = zeros(shape=(3,3))
    for i in range(3):
        for j in range(3):
            peice = imagePixels[i][j].copy()
            peiceCopy = peice.copy()
            # print(f"####### We are on the {peiceNamesRow[i]} {peiceNamesCol[j]} peice ######")
            # imshow("Display window", cvtColor(peiceCopy, COLOR_HSV2BGR)) # CHANGE
            # waitKey(0) # CHANGE
            peiceColor = colorAnalysis(peice)
            face[i][j] = colorsArray[peiceColor]
            # print(f"The {peiceNamesRow[i]} {peiceNamesCol[j]} peice is {COLORS[peiceColor]}")

    return face

def scanFace(colorsArray, readSavedImg=False, filename=""):
    if readSavedImg:
        image = imread(f"{Abbie_img_folder}{filename}.jpg", IMREAD_COLOR) # CHANGE
    else:
        image = captureImg(CUBE_IMG_FOLDER, "cubeFace")
    normalizedImage = cvtColor(image, COLOR_BGR2HSV)
    return analyzeFace(normalizedImage, colorsArray)

def getCenterColor(image):
    centerPiece = image[IMG_SIZE//3:2*(IMG_SIZE)//3, IMG_SIZE//3:2*(IMG_SIZE)//3]
    return colorAnalysis(centerPiece)

# test to see if cube is impossible
def errorCorrection(cubeArray):
    
    return -1
    
#######################################################
####### Format to ShiftCube ###########################
#######################################################
def faceSearch(cubeArray, query):
    for face in cubeArray:
        if face[1][1] == query:
            return face
    return -1

def convertToShiftCube(cubeArray):
    shiftCube = zeros(shape=6, dtype=uint32)
    # face order is the order in which you will save to
    # the shift cube, written in Colorblind notation
    faceOrder = [0, 1, 2, 3, 4, 5]
    for i in range(6):
        face = faceSearch(cubeArray, faceOrder[i])
        faceNum = uint32(0); k = 1; j = 0; c = 0
        while c < 8:
            peice = uint32(face[k][j])
            # print(f"face {i} peice[{k}][{j}] faceNum so far: {faceNum} \n peice: {peice}")
            faceNum = faceNum << (4) # move over a nibble
            faceNum += peice
            # traverse in loop around center
            if j == 0 and k != 2:
                k+=1
            elif k == 2 and j != 2:
                j+=1
            elif j == 2 and k !=0:
                k-=1
            elif k == 0:
                j-=1
            c+=1
        shiftCube[i] = faceNum
    return shiftCube

#######################################################
####### Testing #######################################
#######################################################

def testScan(frontCenterColor, upCenterColor, filename):
    print("------------------------------")
    print("Starting Test!")
    print(f"Generating colors array with {COLORS[frontCenterColor]} facing front and {COLORS[upCenterColor]} facing up")
    colorsArray = genColorsArray(frontCenterColor, upCenterColor)
    print("colors array generated")
    print(f"Colors array: {colorsArray}")
    print("Starting Face Scan!")
    faceArray = scanFace(colorsArray, True, filename)
    print("face scanned!")
    print(faceArray)
    print(translateToColors(faceArray, colorsArray))
    return faceArray

def testFromSaved(frontCenterColor, upCenterColor):
    print("------------------------------")
    print("Starting Test!")
    print(f"Generating colors array with {COLORS[frontCenterColor]} facing front and {COLORS[upCenterColor]} facing up")
    colorsArray = genColorsArray(frontCenterColor, upCenterColor)
    print("colors array generated")
    print(f"Colors array: {colorsArray}")
    print("Starting Cube Scan!")
    cubeArray = []
    imageNames = ["wholecube1", "wholecube2","wholecube3","wholecube4","wholecube5","wholecube6"]
    for i in range(6):
        faceArray = scanFace(colorsArray, True, imageNames[i])
        print(f"face {i} scanned!")
        print(faceArray)
        print(translateToColors(faceArray, colorsArray))
        cubeArray.append(faceArray)
    print("Cube Scanned!!")
    print(cubeArray)
    print(f"Converted to shift cube: {convertToShiftCube(cubeArray)}")

def test(frontCenterColor, upCenterColor):
    print("------------------------------")
    print("Starting Test!")
    print(f"Generating colors array with {COLORS[frontCenterColor]} facing front and {COLORS[upCenterColor]} facing up")
    colorsArray = genColorsArray(frontCenterColor, upCenterColor)
    print("colors array generated")
    print(f"Colors array: {colorsArray}")
    print("Starting Cube Scan!")
    cubeArray = []
    for i in range(6):
        faceArray = scanFace(colorsArray)
        print(f"face {i} scanned!")
        print(faceArray)
        print(translateToColors(faceArray, colorsArray))
        cubeArray.append(faceArray)
    print("Cube Scanned!!")
    print(cubeArray)
    print(f"Converted to shift cube: {convertToShiftCube(cubeArray)}")

testFromSaved(2, 0) # blue, yellow