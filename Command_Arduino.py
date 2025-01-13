import serial

ARDUINO = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=None, write_timeout=None)

def push_robotStrs_to_arduino(strs: list[str]):
    #print(nums)
    ARDUINO.write(f"{strs[0]}\n".enciode('utf-8'))
    index = 1
    while index < len(strs):
        if (ARDUINO.in_waiting > 0):
            ARDUINO.reset_input_buffer()
            if index < len(strs): 
                ARDUINO.write(bytes(strs[index]))
                index += 1

def push_robotNums_to_arduino(nums: list[int]):
    print(nums)
    Bytes = []
    for num in nums:
        Bytes.append(num>>8)
        Bytes.append(num&(2**8 - 1))
    for byte in Bytes: 
        if byte > 255: raise ValueError("BYTE WAS NOT A BYTE!")
    if len(Bytes) == 0: raise ValueError("BYTES WAS NOTHING")
    ARDUINO.write(bytes(Bytes[:2]))
    index = 2
    while index < len(nums):
        if (ARDUINO.in_waiting > 0):
            ARDUINO.reset_input_buffer()
            if index < len(nums)*2: 
                ARDUINO.write(bytes(Bytes[index:index+2]))
                index += 2