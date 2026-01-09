#!/bin/bash
set -euo pipefail

echo "LTA power supply on"
powersupply_on.py

../startup.sh 20
