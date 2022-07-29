#!/bin/sh

OPENWRT_USER=root
OPENWRT_HOST=192.168.247.130

make clean

make

if [ -f ng-sentry.ko ]; then

sftp "$OPENWRT_USER"@"$OPENWRT_HOST" <<!
rm ng-sentry.ko
put ng-sentry.ko
!

fi
