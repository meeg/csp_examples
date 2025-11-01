#!/bin/bash
#first append arguments
msg=""                     
for ((i = 2; i <= $#; i++)) ; do
    msg="$msg ${!i}"
done
remotecli.exe -C 5 -p 14 "echo $msg | nc localhost $1 | tail"
