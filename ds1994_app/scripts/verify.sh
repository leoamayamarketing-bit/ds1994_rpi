#!/bin/bash
# Verificación de registros DS1994-F5 mediante sysfs
# Protocolo sin SKIP_ROM (por datasheet)

DEVICE_ID="${1:-04-00000065968c}"
DEVICE_PATH="/sys/devices/w1_bus_master1/${DEVICE_ID}/rw"

if [ ! -f "$DEVICE_PATH" ]; then
    echo "Dispositivo $DEVICE_ID no encontrado"
    exit 1
fi

echo "=========================================="
echo "  DS1994-F5 Diagnóstico: $DEVICE_ID"
echo "=========================================="

read_byte() {
    local addr_lo="$1"
    local addr_hi="$2"
    echo -ne "\xF0\x${addr_lo}\x${addr_hi}" > "$DEVICE_PATH" 2>/dev/null
    sleep 0.05
    dd if="$DEVICE_PATH" bs=1 count=1 2>/dev/null | xxd -p | tr -d '\n'
}

echo ""
echo "1. STATUS REGISTER (0x0200)"
echo -ne "\xF0\x00\x02" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
STATUS=$(dd if="$DEVICE_PATH" bs=1 count=1 2>/dev/null | xxd -p | tr -d '\n')
echo "   0x${STATUS}"

echo ""
echo "2. CONTROL REGISTER (0x0201)"
echo -ne "\xF0\x01\x02" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
CONTROL=$(dd if="$DEVICE_PATH" bs=1 count=1 2>/dev/null | xxd -p | tr -d '\n')
echo "   0x${CONTROL}"

echo ""
echo "3. RTC (5 bytes en 0x0202)"
echo -ne "\xF0\x02\x02" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
dd if="$DEVICE_PATH" bs=1 count=5 2>/dev/null | xxd -p | sed 's/^/   /'

echo ""
echo "4. Interval Timer (5 bytes en 0x0207)"
echo -ne "\xF0\x07\x02" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
dd if="$DEVICE_PATH" bs=1 count=5 2>/dev/null | xxd -p | sed 's/^/   /'

echo ""
echo "5. Cycle Counter (4 bytes en 0x020C)"
echo -ne "\xF0\x0C\x02" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
dd if="$DEVICE_PATH" bs=1 count=4 2>/dev/null | xxd -p | sed 's/^/   /'

echo ""
echo "6. Primeros 16 bytes de SRAM (0x0000)"
echo -ne "\xF0\x00\x00" > "$DEVICE_PATH" 2>/dev/null
sleep 0.05
dd if="$DEVICE_PATH" bs=1 count=16 2>/dev/null | xxd -p | sed 's/^/   /'
