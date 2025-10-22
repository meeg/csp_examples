#!/bin/bash
remotecli.exe -C 4 "output 6 1 0"
echo "waiting 60 sec for boot"
sleep 60
./serialcli.py "ifconfig eth0 192.168.133.101"
echo "ready"
