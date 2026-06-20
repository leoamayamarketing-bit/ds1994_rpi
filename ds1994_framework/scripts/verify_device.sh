#!/bin/bash
# Full register verification of DS1994-F5
# Protocol: READ_MEMORY without SKIP_ROM (per datasheet)

DEVICE_ID="${1:-04-00000065968c}"
DEVICE_PATH="/sys/devices/w1_bus_master1/${DEVICE_ID}/rw"

if [ ! -f "$DEVICE_PATH" ]; then
    echo "Device $DEVICE_ID not found at $DEVICE_PATH"
    exit 1
fi

echo "=========================================="
echo "  DS1994-F5 Diagnostics: $DEVICE_ID"
echo "=========================================="

read_byte() {
    local addr="$1"
    echo -ne "\xF0\x$(printf '%02x' $((addr & 0xFF)))\x$(printf '%02x' $((addr >> 8)))" > "$DEVICE_PATH" 2>/dev/null
    sleep 0.05
    dd if="$DEVICE_PATH" bs=1 count=1 2>/dev/null | xxd -p | tr -d '\n'
}

echo ""
echo "1. STATUS REGISTER (0x0200)"
echo -ne "\xF0\x00\x02" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
STATUS=$(dd if="$DEVICE_PATH" bs=1 count=1 2>/dev/null | xxd -p | tr -d '\n')
echo "   Value: 0x${STATUS}"
echo "   Bits:  CCE=$(( (0x${STATUS} >> 5) & 1 )) ITE=$(( (0x${STATUS} >> 4) & 1 )) RTE=$(( (0x${STATUS} >> 3) & 1 ))"
echo "          CCF=$(( (0x${STATUS} >> 2) & 1 )) ITF=$(( (0x${STATUS} >> 1) & 1 )) RTF=$(( (0x${STATUS} >> 0) & 1 ))"

echo ""
echo "2. CONTROL REGISTER (0x0201)"
echo -ne "\xF0\x01\x02" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
CONTROL=$(dd if="$DEVICE_PATH" bs=1 count=1 2>/dev/null | xxd -p | tr -d '\n')
CONTROL_VAL=$((0x${CONTROL}))
echo "   Value: 0x${CONTROL}"
echo "   DSEL=$(( (CONTROL_VAL >> 7) & 1 )) STP=$(( (CONTROL_VAL >> 6) & 1 )) AUTO=$(( (CONTROL_VAL >> 5) & 1 ))"
echo "   OSC=$(( (CONTROL_VAL >> 4) & 1 ))  RO=$(( (CONTROL_VAL >> 3) & 1 ))  WPC=$(( (CONTROL_VAL >> 2) & 1 ))"
echo "   WPI=$(( (CONTROL_VAL >> 1) & 1 )) WPR=$(( (CONTROL_VAL >> 0) & 1 ))"

echo ""
echo "3. RTC (0x0202-0x0206)"
echo -ne "\xF0\x02\x02" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
RTC_RAW=$(dd if="$DEVICE_PATH" bs=1 count=5 2>/dev/null | xxd -p | tr -d '\n')
echo "   Raw: 0x${RTC_RAW}"

echo ""
echo "4. WRITE TEST (0xAA @ 0x0000)"
echo -ne "\xF0\x00\x00" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
ORIG=$(dd if="$DEVICE_PATH" bs=1 count=1 2>/dev/null | xxd -p | tr -d '\n')
echo "   Original: 0x${ORIG}"

echo -ne "\x0F\x00\x00\xAA" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
echo -ne "\x55\x00\x00\x00" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05

echo -ne "\xF0\x00\x00" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
AFTER=$(dd if="$DEVICE_PATH" bs=1 count=1 2>/dev/null | xxd -p | tr -d '\n')
echo "   After write: 0x${AFTER}"

if [ "$ORIG" == "$AFTER" ]; then
    echo ""
    echo "   RESULT: Write-protected (value unchanged)"
else
    echo ""
    echo "   RESULT: Write successful"
fi

echo ""
echo "=========================================="
echo "  Diagnostics complete"
echo "=========================================="
