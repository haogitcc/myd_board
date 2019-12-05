#!/bin/sh
ps | grep hostapd | grep -v grep | awk '{print $1}' | while read pid
do
    echo "hosapd is running, to kill hosapd pid=$pid"
    kill -9 $pid
    echo "kill result: $?"
done




