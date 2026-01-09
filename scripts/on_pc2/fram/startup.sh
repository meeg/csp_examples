#!/bin/bash
# when called with optional argument, e.g. "./startup.sh 17", the daemon will connect to an LTA at 192.168.133.17

#scp pi@192.168.133.100:configure.static ~/
nand-attach.sh -f nandp0
DAEMONDIR=/data/nandp0v1
mkdir -p $DAEMONDIR/temp
mkdir -p $DAEMONDIR/images
ln -sf /data/nandp0v1/initfile /etc/init.d/ltaDaemon
ln -sf /data/nandp0v1/conffile /etc/ltaDaemon.conf
if [[ "$#" -eq 1 ]]; then
	echo "daemon will connect to LTA at 192.168.133.$1"
	sed -e "s/\(DAEMONARGS.*192.168.133\)\.[0-9]*/\1.$1/" -i /etc/ltaDaemon.conf
fi
/etc/init.d/ltaDaemon start
echo "startup.sh done"
