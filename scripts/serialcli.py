#!/usr/bin/env python
import sys
import serial

print(sys.argv)
if len(sys.argv) != 2:
    print(sys.argv[0] + " <command string, in quotes if necessary>")
    sys.exit()
cmd = sys.argv[1]
s = serial.Serial('/dev/ttyUSB_rs485', baudrate=115200, timeout=1.0)
s.write((cmd+"\n").encode())
print(b"".join(s.readlines()).decode())
