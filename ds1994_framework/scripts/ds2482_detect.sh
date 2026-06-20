#!/bin/bash
# Detect DS1994-F5 via DS2482 I2C-to-1-Wire bridge
I2C_BUS=1
DS2482_ADDR=0x18

if [[ $EUID -ne 0 ]]; then
    echo "Run as root: sudo $0"
    exit 1
fi

echo "DS2482 Detection on I2C bus $I2C_BUS..."
if ! command -v i2cdetect &>/dev/null; then
    apt-get update && apt-get install -y i2c-tools
fi

if i2cdetect -y $I2C_BUS | grep -q "$(printf '%02x' $DS2482_ADDR)"; then
    echo "DS2482 found at 0x$DS2482_ADDR"
else
    echo "DS2482 NOT found. Check wiring (SDA=GPIO2, SCL=GPIO3, pull-ups)."
    exit 1
fi

modprobe ds2482 2>/dev/null
echo "ds2482 $DS2482_ADDR" > /sys/bus/i2c/devices/i2c-$I2C_BUS/new_device 2>/dev/null
sleep 2

echo ""
echo "1-Wire devices via DS2482:"
for dev in /sys/bus/w1/devices/*/; do
    name=$(basename "$dev")
    if echo "$name" | grep -qE '^(04|24)-'; then
        echo "  DS1994-F5: $name"
    elif echo "$name" | grep -qE '^[0-9a-f]{2}-'; then
        echo "  Other: $name"
    fi
done
