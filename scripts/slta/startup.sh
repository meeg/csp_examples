#!/bin/bash
set -euo pipefail

echo "LTA power supply on"
ssh oscuraLN2 'powersupply_cli.py -c serial -d /dev/ttyUSB_id8 write "INST:NSEL 1"'
ssh oscuraLN2 'powersupply_cli.py -c serial -d /dev/ttyUSB_id8 write "CURR 1.25"'
ssh oscuraLN2 'powersupply_cli.py -c serial -d /dev/ttyUSB_id8 write "VOLT 12.0"'
ssh oscuraLN2 'powersupply_on.py -c serial -d /dev/ttyUSB_id8 1'

../startup.sh 7
