
killhostapd.sh
killudhcpd.sh
killwpa.sh
killudhcpc.sh
ifconfig wlan0 down
ifconfig wlan0 up

echo "shell$0"
echo "p1=$1"
echo "p2=$2"

wpa_passphrase $1 > /opt/boa/cgi-bin/wifi.conf $2
#wpa_passphrase $1 > wifi.conf $2
sleep 3
wpa_supplicant -D wext -B -i wlan0 -c /opt/boa/cgi-bin/wifi.conf
#wpa_supplicant -D wext -B -i wlan0 -c wifi.conf
sleep 1
udhcpc -b -i wlan0 -R &

