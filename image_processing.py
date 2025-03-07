####### IMPORTS #######
from cv2 import imwrite, imread, IMREAD_COLOR, split, normalize,\
NORM_MINMAX, cvtColor, COLOR_BGR2HSV, COLOR_HSV2BGR, merge, inRange,\
imshow, countNonZero, waitKey, bitwise_or, imdecode
# from picamera2 import Picamera2
from numpy import asarray, zeros, argwhere, copy, uint32, sum, rot90
from os import chdir, environ
from subprocess import run

#######################################################
####### Constant Variables            #################
####### (Because hard coding is bad!) #################
#######################################################

Abbie_img_folder = "/home/aagowl/Downloads/"
peiceNamesRow = ["upper", "center", "lower"]
peiceNamesCol = ["left", "center", "right"]
IMG_WIDTH = 56 # for resolution
IMG_HEIGHT = 64
IMG_SIZE = IMG_HEIGHT * IMG_WIDTH
PIECE_SIZE = (IMG_WIDTH//3) * (IMG_HEIGHT//3)
CUBE_IMG_FOLDER = "/home/pi/cubeImgs/"
FACE_ORDER = {"U": 0, "R": 1, "F": 2, "L": 3, "B": 4, "D": 5}
# colors array good for translating numerical colors into english
# does not work when cube translated into colorblind notation (obvi)
# use translateToColors() with a colorsArray to translate colorblind notation
COLORS = ["yellow", "red", "blue", "white", "orange", "green"]
# rows are face, cols are up
RIGHT_FACE_COLOR_ARRAY = [
    #[yellow, red, blue, white, orange, green]
    # y   r   b   w   o   g
    [-1,  2,  4, -1,  5,  1], #yellow is face
    [ 5, -1,  0,  2, -1,  3], #red is face
    [ 1,  3, -1,  4,  0, -1], #blue is face
    [-1,  5,  1, -1,  2,  4], #white is face
    [ 2, -1,  3,  5, -1,  0], #orange is face
    [ 4,  0, -1,  1,  3, -1] #green is face
]

# determines what colors are defined as in HSV
# H is on a scale from 0-180
# S is on a scale from 0-255
# V is on a scale from 0-255
LOWER_BOUND_COLORS = [
    asarray([ 25, 60, 70]), # yellow
    asarray([159, 50, 70]), # red
    asarray([ 90, 50, 70]), # blue
    asarray([  0,  0, 70]), # white
    asarray([  3, 50, 70]), # orange
    asarray([ 36, 50, 70]), # green
]

UPPER_BOUND_COLORS = [
    asarray([ 35, 255, 255]), # yellow
    asarray([180, 255, 255]), # red
    asarray([128, 255, 255]), # blue
    asarray([179,  49, 255]), # white
    asarray([ 24, 255, 255]), # orange
    asarray([ 89, 255, 255]), # green
]

PEICE_CONNECTION_TABLE = [
    # UP
    [[[], [4, 0, 1], []],
    [[3, 0, 1], [], [1, 0, 1]],
    [[], [2, 0, 1], []]],
    # FRONT
    [[[], [0, 2, 1], []],
    [[3, 1, 2], [], [1, 1, 0]],
    [[], [5, 0, 1], []]],
    # DOWN
    [[[], [2, 2, 1], []],
    [[3, 2, 1], [], [1, 2, 1]],
    [[], [5, 2, 1], []]],
    # BACK 
    [[[], [0, 0, 1], []], 
    [[1, 1, 2], [], [3, 1, 0]], 
    [[], [5, 2, 1], []]]
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
    environ["LIBCAMERA_LOG_LEVELS"] = "3"
    # capture image using libcamera with specified resolution
    run([
        "libcamera-still", 
        "-o", 
        f"{filename}.jpg",
        "-v", "0", 
        "--width", 
        f"{IMG_WIDTH}",
        "--height",
        f"{IMG_HEIGHT}",
        "--viewfinder-width", "60", 
        "--viewfinder-height", "60",
        "--immediate"])
    
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
        peiceCopy = peice.copy()
        mask = inRange(peiceCopy, LOWER_BOUND_COLORS[i], UPPER_BOUND_COLORS[i])
        # imshow('Mask', mask) # CHANGE
        # waitKey(0) # CHANGE
        # find the percentage of the piece that is in that color range
        percent = (countNonZero(mask)/(PIECE_SIZE)) * 100
        # if more than 60% of the peice is that color, return the color
        if percent >= 50 or (i == 1 and redColorCheck(peiceCopy, mask) >= 50):
            return i
    # if nothing hits its probably white
    return 3

def redColorCheck(peiceCopy, mask1):
    red2Upper = asarray([2, 255, 255])
    red2Lower = asarray([0, 50, 70])
    mask2 = inRange(peiceCopy, red2Lower, red2Upper)
    result = bitwise_or(mask1, mask2)
    return (countNonZero(result)/(PIECE_SIZE)) * 100

def brightnessCheck(cubeImg):
    # returns true if too dark
    brightnessUpper = asarray([180, 255, 25])
    mask = inRange(cubeImg, asarray([0, 0, 0]), brightnessUpper)
    if ((countNonZero(mask)/(IMG_SIZE)) * 100) >= 50:
        return True
    return False

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
    if upCenterColor + 3 <= 5:
        colorsArray[upCenterColor + 3] = 5
    else:
        colorsArray[upCenterColor - 3] = 5
    #back
    if frontCenterColor + 3 <= 5:
        colorsArray[frontCenterColor + 3] = 4
    else:
        colorsArray[frontCenterColor - 3] = 4
    #left
    if rightCenterColor + 3 <= 5:
        colorsArray[rightCenterColor + 3] = 3
    else:
        colorsArray[rightCenterColor - 3] = 3

    return colorsArray

def translateToColors(faceArray, colorsArray):
    faceColors = [["", "", ""],["", "", ""],["", "", ""]]
    for i in range(3):
        for j in range(3):
            faceColors[i][j] = COLORS[int(argwhere(colorsArray == faceArray[i][j])[0])]
    
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
    # represent face as array 
    face = zeros(shape=(3,3))
    for i in range(3):
        for j in range(3):
            peice = imagePixels[i][j].copy()
            # peiceCopy = peice.copy()
            # print(f"####### We are on the {peiceNamesRow[i]} {peiceNamesCol[j]} peice ######")
            # imshow("Display window", cvtColor(peiceCopy, COLOR_HSV2BGR)) # CHANGE
            # waitKey(0) # CHANGE
            peiceColor = colorAnalysis(peice)
            face[i][j] = colorsArray[peiceColor]
            # print(f"The {peiceNamesRow[i]} {peiceNamesCol[j]} peice is {COLORS[peiceColor]}")
    print(translateToColors(face, colorsArray))
    return face

def scanFace(colorsArray, readSavedImg=False, filename=""):
    if readSavedImg:
        image = imread(f"{CUBE_IMG_FOLDER}{filename}.jpg", IMREAD_COLOR) # CHANGE
    else:
        image = captureImg(CUBE_IMG_FOLDER, "cubeFace")
    normalizedImage = cvtColor(image, COLOR_BGR2HSV)
    #if brightnessCheck(image):
    #    return None
    return analyzeFace(normalizedImage, colorsArray)

def addFaceToCubeScan(faceArray, orientation, cubeArray):
    if faceArray is None or cubeArray is None:
        return None
    # rotate cube properly
    faceArray = rot90(faceArray, k=orientation.rot, axes=(1,0))
    # put in cube array properly
    cubeArray[FACE_ORDER[orientation.face]] = faceArray
    return cubeArray

def getCenterColor(image):
    normalizedImage = cvtColor(image, COLOR_BGR2HSV)
    centerPiece = normalizedImage[IMG_HEIGHT//3:2*(IMG_HEIGHT)//3, IMG_WIDTH//3:2*(IMG_WIDTH)//3]
    return colorAnalysis(centerPiece)

#######################################################
####### ERROR DECTECTION!!! ###########################
#######################################################
# test to see if cube is impossible
# USE BEFORE SHIFT CUBE!!!!!!!
# https://puzzling.stackexchange.com/questions/53846/how-to-determine-whether-a-rubiks-cube-is-solvable
def errorDetection(cubeArray):
    # returns false if invalid cube
    # returns true if no problem detected
    if (sum(cubeArray) != 135):
        return 0
    if (not checkEdgeParity(cubeArray)):
        return 2
    # if (!checkCornerRotations(cubeArray)):
    #     return 3
    # if (!checkPurmutationParity(cubeArray)):
    #     return 4
    return 1

def checkCornerRotations(cubeArray):
    return True

def checkEdgeParity(cubeArray):
    c = 0
    faceToCheck = [0, 2, 4, 5]
    for i in range(4):
        # check top and bottom edges
        peice = cubeArray[faceToCheck[i]][1][0]
        peiceConCords = PEICE_CONNECTION_TABLE[i][1][0]
        peiceCon = cubeArray[peiceConCords[0], peiceConCords[1], peiceConCords[2]]
        c += checkEdge(peice, peiceCon)
        peice = cubeArray[faceToCheck[i]][1][2]
        peiceConCords = PEICE_CONNECTION_TABLE[i][1][2]
        peiceCon = cubeArray[peiceConCords[0], peiceConCords[1], peiceConCords[2]]
        c += checkEdge(peice, peiceCon)
        # only some of the faces need these edges to be checked
        if i % 2 == 0:
            continue
        # check side edges
        peice = cubeArray[faceToCheck[i]][0][1]
        peiceConCords = PEICE_CONNECTION_TABLE[i][0][1]
        peiceCon = cubeArray[peiceConCords[0], peiceConCords[1], peiceConCords[2]]
        c += checkEdge(peice, peiceCon)
        peice = cubeArray[faceToCheck[i]][2][1]
        peiceConCords = PEICE_CONNECTION_TABLE[i][2][1]
        peiceCon = cubeArray[peiceConCords[0], peiceConCords[1], peiceConCords[2]]
        c += checkEdge(peice, peiceCon)
    # must be divisible by two to be a valid cube
    if c % 2 == 0:
        return True
    return False

def checkEdge(peice, peiceCon):
    if peice == 1 or peice == 3:
        return 0
    if peice == 2 or peice == 4:
        return 1
    if peiceCon == 2 or peiceCon == 4:
        return 0
    return 1

def checkPurmutationParity(cubeArray):
    return True
    
#######################################################
####### Format to ShiftCube ###########################
#######################################################
def convertToShiftCube(cubeArray):
    shiftCube = zeros(shape=6, dtype=uint32)
    # face order is the order in which you will save to
    # the shift cube, written in Colorblind notation
    for i in range(6):
        face = asarray(cubeArray[i], dtype=uint32)
        shiftCube[i] |= face[0][0]       | face[0][1] << 4  | face[0][2] << 8
        shiftCube[i] |= face[1][0] << 28 |                    face[1][2] << 12
        shiftCube[i] |= face[2][0] << 24 | face[2][1] << 20 | face[2][2] << 16

    return shiftCube

#######################################################
####### Testing #######################################
#######################################################

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
    cubeArr = []
    for i in range(6):
        cubeArr.append(scanFace(colorsArray))
    print("Cube Scanned!!")
    print(cubeArr)
    print(f"converted to shiftcube {convertToShiftCube(cubeArr)}")

if __name__ == "__main__":
    test(2, 0)
