#!/bin/sh

deallocvt
chvt 1

rm /tmp/vt/gmenu2x 2>/dev/null
mkdir /tmp/apps 2>/dev/null
mkdir /tmp/vt 2>/dev/null

source /etc/profile
trap "" hup
clear
exec /usr/bin/gmenu2x.bin
