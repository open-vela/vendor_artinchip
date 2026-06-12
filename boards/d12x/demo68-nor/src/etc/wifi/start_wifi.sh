#!/bin/sh
set +e

echo "[wifi] start_wifi"

WAPI_CONF="/data/etc/wifi/wapi.conf"

#  Read parameters from config file (lowercase fields)
SSID=$(grep -oP '"ssid":\s*"\K[^"]+' $WAPI_CONF)
PSK=$(grep -oP '"psk":\s*"\K[^"]+' $WAPI_CONF)
BSSID=$(grep -oP '"bssid":\s*"\K[^"]+' $WAPI_CONF)

echo "[wifi] SSID=$SSID  BSSID=$BSSID"

# Disconnect
wapi disconnect wlan0
sleep 1

# Set SSID
if [ -n "$SSID" ]; then
    echo "[wifi] setting ssid: $SSID"
    wapi essid wlan0 "$SSID" 1
fi

# Set PSK (WPA2)
if [ -n "$PSK" ]; then
    echo "[wifi] setting psk"
    wapi psk wlan0 "$PSK" 1 3
fi

# Set fixed BSSID（5GHz）
if [ -n "$BSSID" ]; then
    echo "[wifi] setting bssid: $BSSID"
    wapi ap wlan0 $BSSID
fi

# Lock 5GHz frequency（Channel 44 → 5220MHz）
#echo "[wifi] lock 5GHz freq 5220"
#wapi freq wlan0 5220 1

# Disable adaptive mode
echo "[wifi] disable adaptive mode"
wapi private wlan0 adaptivity 0

# Disable power save
echo "[wifi] disable power save"
wapi power_save wlan0 off

# Save config
wapi save_config wlan0

# Reconnect
echo "[wifi] reconnecting..."
wapi reconnect wlan0

# Wait for WLAN ready
sleep 5

# Try DHCP（try up to 3 times）
for i in 1 2 3; do
    echo "[wifi] DHCP renew try $i"
    renew wlan0
    if [ "$?" = "0" ]; then
        break
    fi
    sleep 2
done

# Show final config
wapi show wlan0
echo "[wifi] wifi startup done"