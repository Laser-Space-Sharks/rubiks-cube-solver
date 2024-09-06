import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BOARD)
GPIO.setup(37, GPIO.OUT)
toggleD = GPIO.PWM(37,50)
toggleD.start(10)
time.sleep(0.5)
toggleD.ChangeDutyCycle(2)
time.sleep(0.5)
toggleD.stop()
GPIO.cleanup()
