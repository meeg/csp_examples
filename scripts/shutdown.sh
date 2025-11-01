#!/bin/bash
echo "LTA power supply off"
powersupply_off.py
echo "PC2 shutdown (this command will time out)"
remotecli.exe -C 5 -t 1000 "bdr sys poweroff"
echo "waiting 10 sec for shutdown"
sleep 10
remotecli.exe -C 4 "output 6 0 0"
echo "done"
