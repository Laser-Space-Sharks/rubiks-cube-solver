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
import numpy as np
import matplotlib as plt

####### NORMALIZE #######
def normalizeImg(image):
    # split into color channels
    b, g, r = cv2.split(image)

    # normalize channels
    bNorm = cv2.normalize(b, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX)
    gNorm = cv2.normalize(g, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX)
    rNorm = cv2.normalize(r, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX)
    
    # merge
    normalizedImage = cv2.merge([rNorm, gNorm, bNorm])

    return normalizedImage
