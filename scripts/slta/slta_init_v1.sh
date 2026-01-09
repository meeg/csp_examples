IMGFOLDER="${IMGFOLDER:-./images}"
CDSOUT="${CDSOUT:-2}"
lta nomulti
source ./slta_clocks_v1.sh
lta set sinit 110
lta set pinit 0
lta set ssamp 200
lta set psamp 200
lta set packSource 9
lta set cdsout "$CDSOUT"
#mkdir -p "$IMGFOLDER"
