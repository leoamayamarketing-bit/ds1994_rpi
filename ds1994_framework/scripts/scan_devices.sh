#!/bin/bash
# Scan 1-Wire bus for DS1994 devices (family code 04)
BUS_PATH="/sys/devices/w1_bus_master1"

if [ ! -d "$BUS_PATH" ]; then
    echo "1-Wire bus not found at $BUS_PATH"
    echo "Run: sudo modprobe w1-gpio w1-therm"
    exit 1
fi

echo "Forcing bus search..."
echo 1 | sudo tee "$BUS_PATH/w1_master_search" > /dev/null 2>&1
sleep 2

echo ""
echo "Devices on bus:"
for dev in "$BUS_PATH"/*/; do
    name=$(basename "$dev")
    if echo "$name" | grep -qE '^[0-9a-f]{2}-'; then
        echo "  $name"
    fi
done

echo ""
echo "DS1994 devices (family 04):"
ls "$BUS_PATH"/ 2>/dev/null | grep '^04-' | while read dev; do
    echo "  $dev"
    if [ -f "$BUS_PATH/$dev/rw" ]; then
        echo "    -> rw interface available"
    fi
done
