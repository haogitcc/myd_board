#!/bin/sh
ps | grep udhcpd | grep -v grep | awk '{print $1}' | while read pid
do
    echo "udhcpd is running, to kill udhcpd pid=$pid"
    kill -9 $pid
    echo "kill result: $?"
done

ifconfig wlan0 down
