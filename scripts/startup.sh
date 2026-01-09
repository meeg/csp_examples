#!/bin/bash
# turn on PC2 and start the LTA daemon
# when called with optional argument, e.g. "./startup.sh 17", the daemon will connect to an LTA at 192.168.133.17
set -euo pipefail

echo "enabling CAN interface"
ip link show can0
if [ $? -ne 0 ]; then
    sudo ip link set can0 up type can bitrate 1000000
fi

echo "PC2 on"
remotecli.exe -C 4 "output 6 1 0"
echo "waiting 60 sec for PC2 boot"
sleep 60

#./serialcli.py "ifconfig eth0 192.168.133.101"
remotecli.exe -C 5 -p 14 "ifconfig eth0 192.168.133.101"
echo "setting up and starting daemon"
#./serialcli.py "/data/fram/startup.sh"
set +u
remotecli.exe -C 5 -p 14 "/data/fram/startup.sh $1"
set -u
echo "waiting 30 sec for LTA boot"
sleep 30
echo "ready"
