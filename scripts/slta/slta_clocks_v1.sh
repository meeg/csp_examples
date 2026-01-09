#!/bin/bash
vh=9
vl=-2.5
th=8.5
tl=-3
hh=8
hl=-3.5
sh=7.5
sl=-10
oh=1.5
ol=-8
rh=7
rl=-4
dh=-1
dl=-10
port="${daemonport:-${PORT:-8888}}"
./lta.sh "$port" set v1ah $vh
./lta.sh "$port" set v1al $vl
./lta.sh "$port" set v1bh $vh
./lta.sh "$port" set v1bl $vl
./lta.sh "$port" set v2ah $vh
./lta.sh "$port" set v2al $vl
./lta.sh "$port" set v2bh $vh
./lta.sh "$port" set v2bl $vl
./lta.sh "$port" set v3ah $vh
./lta.sh "$port" set v3al $vl
./lta.sh "$port" set v3bh $vh
./lta.sh "$port" set v3bl $vl
./lta.sh "$port" set h1ah $hh
./lta.sh "$port" set h1al $hl
./lta.sh "$port" set h1bh $hh
./lta.sh "$port" set h1bl $hl
./lta.sh "$port" set h2ah $hh
./lta.sh "$port" set h2al $hl
./lta.sh "$port" set h2bh $hh
./lta.sh "$port" set h2bl $hl
./lta.sh "$port" set h3ah $hh
./lta.sh "$port" set h3al $hl
./lta.sh "$port" set h3bh $hh
./lta.sh "$port" set h3bl $hl
./lta.sh "$port" set swah $sh
./lta.sh "$port" set swal $sl
./lta.sh "$port" set swbh $dh
./lta.sh "$port" set swbl $dl
./lta.sh "$port" set rgah $rh
./lta.sh "$port" set rgal $rl
./lta.sh "$port" set rgbh -7
./lta.sh "$port" set rgbl -7
./lta.sh "$port" set ogah $oh
./lta.sh "$port" set ogal $ol
./lta.sh "$port" set ogbh $oh
./lta.sh "$port" set ogbl $ol
./lta.sh "$port" set tgah $th
./lta.sh "$port" set tgal $tl
./lta.sh "$port" set tgbh $th
./lta.sh "$port" set tgbl $tl
./lta.sh "$port" set vdrain -22
./lta.sh "$port" set vdd -22
./lta.sh "$port" set vr -22
./lta.sh "$port" set vsub 40
./lta.sh "$port" set vdrain_sw 0
./lta.sh "$port" set vdd_sw 1
./lta.sh "$port" set vsub_sw 1
./lta.sh "$port" set vr_sw 1
./lta.sh "$port" set p15v_sw 1
./lta.sh "$port" set m15v_sw 1
