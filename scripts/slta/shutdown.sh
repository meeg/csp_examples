#!/bin/bash
set -euo pipefail

echo "LTA power supply off"
ssh oscuraLN2 'powersupply_off.py -c serial -d /dev/ttyUSB_id8 1'

../shutdown.sh
echo "PC2 shutdown (this command will time out)"
remotecli.exe -C 5 -t 1000 "bdr sys poweroff"

echo "waiting 10 sec for shutdown"
sleep 10

remotecli.exe -C 4 "output 6 0 0"
echo "done"
