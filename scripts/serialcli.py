#!/usr/bin/env python
import sys
import getopt
import serial

timeout = 1.0

def print_help():
    print("\nUsage: "+sys.argv[0]+" <command string, in quotes if necessary>")
    print("Arguments: ")
    print("\t-t: timeout (sec), default 1.0")
    print("\t-h: print this message")
    sys.exit(0)

options, remainder = getopt.gnu_getopt(sys.argv[1:], 't:h')
for opt, arg in options:
    if opt == '-t':
        timeout = float(arg)
    elif opt == '-h':
        print_help()

if len(remainder) != 1:
    print_help()

cmd = remainder[0]
print("command:", cmd)
s = serial.Serial('/dev/ttyUSB_rs485', baudrate=115200, timeout=timeout)
s.write((cmd+"\n").encode())

res = True
while res:
    res = s.read()
    sys.stdout.write(res.decode())
    sys.stdout.flush()
print()
