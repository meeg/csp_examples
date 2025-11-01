source setup_lta.sh
source voltage_skp_lta_v2.sh
lta nomulti
lta sseq sequencer.xml
lta set sinit 30
lta set pinit 0
lta set ssamp 200
lta set psamp 200
lta set packSource 9
lta set cdsout 2 #sig-ped
lta name "images/image_lta_"
lta NROW 1

echo "reading image: the CSP remotecli will always time out and print ERROR, don't worry about it"
lta read

remotecli.exe -C 5 -p 14 "ls -ltr /data/nandp0v1/images"
