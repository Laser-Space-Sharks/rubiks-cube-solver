import os
import time
with open("scan.txt") as file:
    for line in file.readlines():
        pattern = "Nmap scan report for "
        time.sleep(0.2)
        if line[:len(pattern)] == pattern:
            ip_address = line[len(pattern):].strip("()")
            command = f"ssh pi@{ip_address}"
            try:
                os.system(command)
                exit()
            except:
                pass
