#!/bin/bash
set -euo pipefail

echo "LTA power supply off"
ssh oscuraLN2 'powersupply_off.py -c serial -d /dev/ttyUSB_id8 1'

../shutdown.sh
