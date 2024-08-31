import time
import RPi.GPIO as GPIO
#Set GPIO numbering method
GPIO.setmode(GPIO.BOARD)

#Set pin the 8 servo pins to output
GPIO.setup(11, GPIO.OUT)
GPIO.setup(12, GPIO.OUT)
GPIO.setup(13, GPIO.OUT)
GPIO.setup(15, GPIO.OUT)
GPIO.setup(35, GPIO.OUT)
GPIO.setup(36, GPIO.OUT)
GPIO.setup(37, GPIO.OUT)
GPIO.setup(38, GPIO.OUT)

# Note: Direction is relative to the camera
# PWM with pulse of 0 means pulse off
        #       (pin#, 50Hz)
servoList = [GPIO.PWM(12,50), # R
            GPIO.PWM(11,50), # L
            GPIO.PWM(13,50), # U
            GPIO.PWM(15,50), # D
            GPIO.PWM(36,50), # Re
            GPIO.PWM(35,50), # Le
            GPIO.PWM(37,50), # Ue
            GPIO.PWM(38,50)] # De


def reset():
    '''
    This will reset all the arms to be disengaged and at 0 degrees.
    '''
    referenceList = [2,2,2,2,10,2,10,2]
    for i in range(4, 8): servoList[i].ChangeDutyCycle(referenceList[i])
    time.sleep(1)
    for i in range(4): servoList[i].ChangeDutyCycle(referenceList[i])
    time.sleep(1)
    for servo in servoList: servo.ChangeDutyCycle(0)
    return referenceList

referenceList = reset()

def test_REs():
    '''
    This will first make sure the R arm is initially disengaged.
    It will then make the R arm engage and disengage once each, with signals sent 1 second apart.
    '''
    servoList[4].ChangeDutyCycle(10)
    time.sleep(1)
    servoList[4].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[4].ChangeDutyCycle(10)
    time.sleep(1)
    servoList[4].ChangeDutyCycle(0)
def test_LEs():
    '''
    This will first make sure the L arm is initially disengaged.
    It will then make the L arm engage and disengage once each, with signals sent 1 second apart.
    '''
    servoList[5].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[5].ChangeDutyCycle(10)
    time.sleep(1)
    servoList[5].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[5].ChangeDutyCycle(0)
def test_UEs():
    '''
    This will first make sure the U arm is initially disengaged.
    It will then make the U arm engage and disengage once each, with signals sent 1 second apart.
    '''
    servoList[6].ChangeDutyCycle(10)
    time.sleep(1)
    servoList[6].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[6].ChangeDutyCycle(10)
    time.sleep(1)
    servoList[6].ChangeDutyCycle(0)
def test_DEs():
    '''
    This will first make sure the D arm is initially disengaged.
    It will then make the D arm engage and disengage once each, with signals sent 1 second apart.
    '''
    servoList[7].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[7].ChangeDutyCycle(10)
    time.sleep(1)
    servoList[7].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[7].ChangeDutyCycle(0)

def test_Ds():
    '''
    This will make sure that the D-spin servo is initially at 0 degrees.
    It will then move to 90, back to 0, to 180, and back to 0, all with signals sent 1 second apart.
    This means that D, D', D2, and D2' will be tested all in one go.
    '''
    servoList[3].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[3].ChangeDutyCycle(7)
    time.sleep(1)
    servoList[3].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[3].ChangeDutyCycle(12)
    time.sleep(1)
    servoList[3].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[3].ChangeDutyCycle(0)

def test_cube_rotations():
    '''
    This will pre-move the spin servos such that when the arms engage, they can execute cube rotations.
    Make sure to first be holding the cube in the correct position before the R and L arms grab it from you.
    These cube rotations will be executed in this order: X, X', Y, Y', Y2, Y2'
    At the end, the U and D arms will disengage, and the cube should follow the D arm downward.
    '''
    servoList[0].ChangeDutyCycle(2)
    servoList[1].ChangeDutyCycle(7)
    servoList[2].ChangeDutyCycle(2)
    servoList[3].ChangeDutyCycle(12)
    time.sleep(1)
    servoList[4].ChangeDutyCycle(2)
    servoList[5].ChangeDutyCycle(10)
    time.sleep(1)
    servoList[0].ChangeDutyCycle(7)
    servoList[1].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[0].ChangeDutyCycle(2)
    servoList[1].ChangeDutyCycle(7)
    time.sleep(1)
    servoList[6].ChangeDutyCycle(2)
    servoList[7].ChangeDutyCycle(10)
    time.sleep(1)
    servoList[4].ChangeDutyCycle(10)
    servoList[5].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[2].ChangeDutyCycle(7)
    servoList[3].ChangeDutyCycle(7)
    servoList[4].ChangeDutyCycle(0)
    servoList[5].ChangeDutyCycle(0)
    servoList[0].ChangeDutyCycle(2)
    servoList[1].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[0].ChangeDutyCycle(0)
    servoList[1].ChangeDutyCycle(0)
    servoList[2].ChangeDutyCycle(2)
    servoList[3].ChangeDutyCycle(12)
    time.sleep(1)
    servoList[2].ChangeDutyCycle(12)
    servoList[3].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[2].ChangeDutyCycle(2)
    servoList[3].ChangeDutyCycle(12)
    time.sleep(1)
    servoList[2].ChangeDutyCycle(0)
    servoList[3].ChangeDutyCycle(0)
    servoList[6].ChangeDutyCycle(10)
    servoList[7].ChangeDutyCycle(2)
    time.sleep(1)
    servoList[6].ChangeDutyCycle(0)
    servoList[7].ChangeDutyCycle(0)
