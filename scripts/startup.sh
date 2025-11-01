#!/bin/bash
echo "PC2 on"
remotecli.exe -C 4 "output 6 1 0"
echo "LTA power supply on"
powersupply_on.py
echo "waiting 60 sec for PC2 boot"
sleep 60
#./serialcli.py "ifconfig eth0 192.168.133.101"
remotecli.exe -C 5 -p 14 "ifconfig eth0 192.168.133.101"
#echo "waiting 5 sec for LTA to get online"
#sleep 5
echo "setting up and starting daemon"
remotecli.exe -C 5 -p 14 "/data/fram/startup.sh"
#./serialcli.py "/data/fram/startup.sh"
echo "waiting 30 sec for LTA boot"
sleep 30
echo "ready"
