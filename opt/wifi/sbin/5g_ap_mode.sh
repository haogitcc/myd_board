#AP模式 
killwpa.sh
killudhcpc.sh
killhostapd.sh
killudhcpd.sh
ifconfig wlan0 down
ifconfig wlan0 up 
hostapd /etc/hostapd/rtl_hostapd_5G.conf -B &
sleep 3
ifconfig wlan0 192.168.2.1 
udhcpd -fS /etc/udhcpd.conf &
