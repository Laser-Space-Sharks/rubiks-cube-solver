import RPi.GPIO as GPIO
import time
#Set GPIO numbering method
GPIO.setmode(GPIO.BOARD)
#Set pin the 8 servo pins to output
GPIO.setup(15, GPIO.OUT)

D = GPIO.PWM(15,50)

D.ChangeDutyCycle(2)
time.sleep(3)
D.ChangeDutyCycle(10)

'''
        #       (pin#, 50Hz)
servoList = [GPIO.PWM(12,50), # R
            GPIO.PWM(11,50), # L
            GPIO.PWM(13,50), # U
            GPIO.PWM(15,50), # D
            GPIO.PWM(36,50), # Re
            GPIO.PWM(35,50), # Le
            GPIO.PWM(37,50), # Ue
            GPIO.PWM(38,50)] # De
'''
