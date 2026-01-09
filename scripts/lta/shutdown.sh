#!/bin/bash
set -euo pipefail

echo "LTA power supply off"
powersupply_off.py

../shutdown.sh
