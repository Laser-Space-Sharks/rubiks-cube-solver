import os
with open("scan.txt") as file:
    for line in file.readlines():
        pattern = "Nmap scan report for "
        if line[:len(pattern)] == pattern:
            ip_address = line[len(pattern):]
            command = f"ssh pi@{ip_address}"
            try:
                os.system(command)
                exit()
            except:
                pass
