#!/bin/sh
ps | grep udhcpc | grep -v grep | awk '{print $1}' | while read pid
do
    echo "udhcpc is running, to kill udhcpc pid=$pid"
    kill -9 $pid
    echo "kill result: $?"
done

ifconfig wlan0 down
