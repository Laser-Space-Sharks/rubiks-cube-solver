import serial

ARDUINO = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=1)

def push_robotNums_to_arduino(nums: list[int]):
    num_bits = len(nums)*16
    num_bytes = num_bits//8
    total_num = 0
    for num in nums:
        total_num <<= 16
        total_num |= (num<<4)
    Bytes = [0 for _ in range(num_bytes)]
    for i in range(num_bits):
        Bytes[i//8] <<= 1
        Bytes[i//8] |= (total_num>>(num_bits-1-i))&1
    for byte in Bytes: 
        if byte > 255: raise ValueError("BYTE WAS NOT A BYTE!")
    ARDUINO.write(bytes(Bytes[:2]))
    index = 2
    while index < len(nums):
        if (ARDUINO.in_waiting > 0):
            ARDUINO.reset_input_buffer()
            if index < len(nums)*2: 
                ARDUINO.write(bytes(Bytes[index:index+2]))
                index += 2