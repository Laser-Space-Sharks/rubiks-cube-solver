#Import libraries
import RPi.GPIO as GPIO
import time

testString = "F2 L'.R B U.D"

#Set GPIO numbering method
GPIO.setmode(GPIO.BOARD)

#Set pin the 8 servo pins to output
GPIO.setup(11, GPIO.OUT)

#Note: First is the pin, 50 is 50hz for servo. Direction is relative to the camera
# Sets up 8 variables for the servos
servoL = GPIO.PWM(11,50)
servoR = GPIO.PWM(12,50)
servoU = GPIO.PWM(13,50)
servoD = GPIO.PWM(15,50)
#Toggle Servos that engage and disengage the rotating servos
toggleL = GPIO.PWM(35,50)
toggleR = GPIO.PWM(36,50)
toggleU = GPIO.PWM(37,50)
toggleD = GPIO.PWM(38,50)

#PWM with pulse of 0 means pulse off

#The String that is fed is made into an array split between each space
commandList = testString.split()
for cmd in commandList:
    #Splits commands at '.' for commands that happen together
    SyncedCmdList = cmd.split('.')
    for i in SyncedCmdList:
        match str(i): 	
            case "R":
                print("One")
            case "R'":
                print("Two")
            case "L":
                print("Three")
            case "L'":
                print("yo")
            case "U":
                print("l")
            case "U'":
                print("l")
            case "D":
                print("l")
            case "D'":
                print("l")
            case "F":
                print("l")
            case "F'":
                print("l")
            case "B":
                print("l")
            case "B'":
                print("l")
            case "R:E":
                print("l")
            case "R:E'":
                print("l")
            case "L:E":
                print("l")
            case "L:E'":
                print("l")
            case "U:E":
                print("l")
            case "U:E'":
                print("l")
            case "D:E":
                print("l")
            case "D:E'":
                print("l")
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
