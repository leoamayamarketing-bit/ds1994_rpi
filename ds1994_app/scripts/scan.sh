#!/bin/bash
# Escaneo del bus 1-Wire para dispositivos DS1994
BUS_PATH="/sys/devices/w1_bus_master1"

if [ ! -d "$BUS_PATH" ]; then
    echo "Bus 1-Wire no encontrado. Ejecute: sudo modprobe w1-gpio w1-therm"
    exit 1
fi

echo "Forzando búsqueda en el bus..."
echo 1 | sudo tee "$BUS_PATH/w1_master_search" > /dev/null 2>&1
sleep 2

echo ""
echo "Dispositivos DS1994 (código de familia 04):"
ls "$BUS_PATH"/ 2>/dev/null | grep '^04-' | while read dev; do
    echo "  📟 $dev"
    if [ -f "$BUS_PATH/$dev/rw" ]; then
        echo "     → interfaz rw disponible"
    fi
done
