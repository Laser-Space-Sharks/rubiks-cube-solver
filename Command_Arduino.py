import serial

ARDUINO = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=1)

def push_robotNums_to_arduino(nums: list[int]):
    num_bits = len(nums)*12
    num_bytes = (num_bits + 4)//8
    total_num = 0
    for num in nums:
        total_num <<= 16
        total_num |= (num<<4)
    Bytes = [0 for _ in range(num_bytes)]
    for i in range(num_bits):
        Bytes[i//8] <<= 1
        Bytes[i//8] |= (total_num>>(num_bits-1-i))&1
    if (num_bytes & 1 == 1): Bytes[-1] <<= 4
    for byte in Bytes: 
        if byte > 256: raise ValueError("BYTE WAS NOT A BYTE!")
    ARDUINO.write(bytes(Bytes))