#Import libraries
import RPi.GPIO as GPIO
import time

#Set pin 11 to output
GPIO.setup(0, GPIO.OUT)

#Note: 11 is the pin, 50 is 50hz for servo
servoOne = GPIO.PWM(0,50)

#PWM with pulse of 0 means pulse off
servoOne.start(0)

#Define variable duty
duty = 2
#Loop for duty values 2-12 (0-180 degrees respectively)
while duty <= 12:
    servoOne.ChangeDutyCycle(duty)
    time.sleep(0.5)
    duty += 1

#Clean things up at the end
servoOne.stop()
GPIO.cleanup()
print("Goodbye!")