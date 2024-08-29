import RPi.GPIO as GPIO
#Set GPIO numbering method
GPIO.setmode(GPIO.BOARD)
#Set pin the 8 servo pins to output
GPIO.setup(15, GPIO.OUT)

D = GPIO.PWM(15,50)

D.ChangeDutyCycle(2)
D.ChangeDutyCycle(10)
