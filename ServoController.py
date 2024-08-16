#Import libraries
import RPi.GPIO as GPIO
import time

testString = "F2 L'.R B U.D"

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

#Note: First is the pin, 50 is 50hz for servo. Direction is relative to the camera
# Sets up 8 variables for the servos
servoR = GPIO.PWM(12,50)
servoL = GPIO.PWM(11,50)
servoU = GPIO.PWM(13,50)
servoD = GPIO.PWM(15,50)
#Toggle Servos that engage and disengage the rotating servos
toggleR = GPIO.PWM(36,50)
toggleL = GPIO.PWM(35,50)
toggleU = GPIO.PWM(37,50)
toggleD = GPIO.PWM(38,50)
#PWM with pulse of 0 means pulse off

servoList = [servoR, servoL, servoU, servoD, toggleR, toggleL, toggleU, toggleD]
referenceList = [2,2,2,2,10,2,10,2]
#Called to set servos into default state
for i in range(8):
    servoList[i].ChangeDutyCycle(referenceList[i])

def servo_single_turn(servoIndex, b_clockwise):
    if (b_clockwise):
        if (referenceList[servoIndex] == 12):
            print("bad")
        else:
            servoList[servoIndex].ChangeDutyCycle(referenceList[servoIndex]referenceList[servoIndex]+5)
            referenceList[servoIndex] += 5
    else:
        if (referenceList[servoIndex] == 2):
            print("bad")
        else:
            servoList[servoIndex].ChangeDutyCycle(referenceList[servoIndex]-5)
            referenceList[servoIndex] -= 5

def servo_full_turn(servoIndex):
    if (referenceList[servoIndex] == 7):
        print("bad")
    elif (referenceList[0] == 2):
        servoList[servoIndex].ChangeDutyCycle(referenceList[servoIndex]+10)
        referenceList[0] += 10
    else:
        servoList[servoIndex].ChangeDutyCycle(referenceList[servoIndex]-10)
        referenceList[0] -= 10

def servo_engage(servoIndex, b_clockwise):
        if (b_clockwise):
            servoList[servoIndex].ChangeDutyCycle(10)
            referenceList[servoIndex] = 10
        else:
            servoList[servoIndex].ChangeDutyCycle(2)
            referenceList[servoIndex] = 2
def servo_disengage(servoIndex, b_clockwise):
        if (b_clockwise):
            servoList[servoIndex].ChangeDutyCycle(2)
            referenceList[servoIndex] = 2
        else:
            servoList[servoIndex].ChangeDutyCycle(10)
            referenceList[servoIndex] = 10



#The String that is fed is made into an array split between each space
for cmd in testString.split():
    #Splits commands at '.' for commands that happen together
    for i in cmd.split('.'):
        match str(i): 	
            case "R":
                servo_single_turn(0, True)
            case "R'":
                servo_single_turn(0, False)
            case "R2":
                servo_full_turn(0)
            case "L":
                servo_single_turn(1, True)
            case "L'":
                servo_single_turn(1, False)
            case "L2":
                servo_full_turn(1)
            case "U":
                servo_single_turn(2, True)
            case "U'":
                servo_single_turn(2, False)
            case "U2":
                servo_full_turn(2)
            case "D":
                servo_single_turn(3, True)
            case "D'":
                servo_single_turn(3, False)
            case "D2":
                servo_full_turn(3)
            case "F":
                print("Face bad")
            case "F'":
                print("Face bad")
            case "F2":
                print("Face bad")
            case "B":
                print("Face bad")
            case "B'":
                print("Face bad")
            case "B2":
                print("Face bad")
            case "R:E":
                servo_engage(4, False)
            case "R:E'":
                servo_disengage(4, False)
            case "L:E":
                servo_engage(5, True)
            case "L:E'":
                servo_disengage(5, True)
            case "U:E":
                servo_engage(6, False)
            case "U:E'":
                servo_disengage(6, False)
            case "D:E":
                servo_engage(7, True)
            case "D:E'":
                servo_disengage(7, True)
            case _:
                print("Invalid Command!" + str(i))
    time.sleep()
    #Call on the servo that is moving after it reaches its destination to stop its jittering
    #servo.ChangeDutyCycle(0)

#Loop for duty values 2-12 (0-180 degrees respectively)
'''for duty in range(2, 13):
    servoOne.ChangeDutyCycle(duty)
    time.sleep(0.3)
    servoOne.ChangeDutyCycle(0)
    time.sleep(0.7)
'''

#Clean things up at the end
servoOne.stop()
GPIO.cleanup()
print("Goodbye!")
