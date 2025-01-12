import serial

ARDUINO = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)

def push_robotNums_to_arduino(nums: list[int]):
    num_bits = len(nums)*12
    num_bytes = (num_bits + 4)//8
    total_num = 0
    for num in nums:
        total_num <<= 12
        total_num |= num
    bytes = [0 for _ in range(num_bytes)]
    for i in range(num_bits):
        bytes[i//8] <<= 1
        bytes[i//8] |= (total_num>>(num_bits-1-i))&1
    if (num_bytes & 1 == 1): bytes[-1] <<= 4
    ARDUINO.write(bytes, num_bytes)