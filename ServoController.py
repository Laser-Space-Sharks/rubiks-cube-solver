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

#The String that is fed is made into an array split between each space
commandList = testString.split()
for cmd in commandList:
    #Splits commands at '.' for commands that happen together
    SyncedCmdList = cmd.split('.')
    for i in SyncedCmdList:
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
                print("bad")
            case "F'":
                print("bad")
            case "F2":
                print("bad")
            case "B":
                print("bad")
            case "B'":
                print("bad")
            case "B2":
                print("bad")
            case "R:E":
                print("bad")
            case "R:E'":
                print("bad")
            case "L:E":
                print("bad")
            case "L:E'":
                print("bad")
            case "U:E":
                print("bad")
            case "U:E'":
                print("bad")
            case "D:E":
                print("bad")
            case "D:E'":
                print("bad")
            case _:
                print("Invalid Command!" + str(i))
    time.sleep()

#Loop for duty values 2-12 (0-180 degrees respectively)
for duty in range(2, 13):
    servoOne.ChangeDutyCycle(duty)
    time.sleep(0.3)
    servoOne.ChangeDutyCycle(0)
    time.sleep(0.7)

#Clean things up at the end
servoOne.stop()
GPIO.cleanup()
print("Goodbye!")
