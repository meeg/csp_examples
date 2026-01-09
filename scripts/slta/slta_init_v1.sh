#!/bin/bash
EXPOSURE="${EXPOSURE:-10}"
NSAMP="${NSAMP:-1}"
IMGFOLDER="${IMGFOLDER:-./images}"
TOTNCOL="${TOTNCOL:-1200}"
TOTNROW="${TOTNROW:-1350}"
CDSOUT="${CDSOUT:-2}"
source setup_lta.sh
lta nomulti
source ./slta_clocks_v1.sh
lta set sinit 110
lta set pinit 0
lta set ssamp 200
lta set psamp 200
lta set packSource 9
lta set cdsout "$CDSOUT"
lta NCOL "$TOTNCOL"
lta NROW "$TOTNROW"
lta NSAMP "$NSAMP"
lta EXPOSURE "$EXPOSURE"
mkdir -p "$IMGFOLDER"
