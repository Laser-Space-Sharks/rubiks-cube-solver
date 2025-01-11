import serial

ARDUINO = serial.Serial(port='COM4', baudrate=9600, timeout=.1)

def push_robotNums_to_arduino(nums: list[int]):
    bitstring = ''.join([str((num>>i)&1) for num in nums for i in range(12)])
    ARDUINO.write(bytes([int(bitstring[i:i+8], 2) for i in range(0, len(bitstring), 8)]))