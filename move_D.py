import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)
GPIO.setup(38, GPIO.OUT)
toggleD = GPIO.PWM(38,50)
toggleD.ChangeDutyCycle(10)
toggleD.stop()
GPIO.cleanup()
