#!/bin/sh
ps | grep wpa_supplicant | grep -v grep | awk '{print $1}' | while read pid
do
    echo "wpa_supplicant is running, to kill wpa_supplicant pid=$pid"
    kill -9 $pid
    echo "kill result: $?"
done

