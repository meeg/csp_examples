#!/bin/bash
remotecli.exe -C 5 "bdr sys poweroff"
echo "waiting 10 sec for shutdown"
sleep 10
remotecli.exe -C 4 "output 6 0 0"
echo "done"
