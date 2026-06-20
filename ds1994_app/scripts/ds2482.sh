#!/bin/bash
# Detectar DS1994-F5 vía DS2482 I2C-to-1-Wire bridge
I2C_BUS=1
DS2482_ADDR=0x18

if [[ $EUID -ne 0 ]]; then
    echo "Ejecutar como root: sudo $0"
    exit 1
fi

echo "Detectando DS2482 en I2C bus $I2C_BUS..."
if ! command -v i2cdetect &>/dev/null; then
    apt-get update && apt-get install -y i2c-tools
fi

if i2cdetect -y $I2C_BUS | grep -q "$(printf '%02x' $DS2482_ADDR)"; then
    echo "DS2482 encontrado en 0x$DS2482_ADDR"
else
    echo "DS2482 NO encontrado. Verificar: SDA=GPIO2, SCL=GPIO3, pull-ups 4.7kΩ"
    exit 1
fi

modprobe ds2482 2>/dev/null
echo "ds2482 $DS2482_ADDR" > /sys/bus/i2c/devices/i2c-$I2C_BUS/new_device 2>/dev/null
sleep 2

echo ""
echo "Dispositivos 1-Wire vía DS2482:"
ls /sys/bus/w1/devices/ 2>/dev/null | grep -v "w1_bus_master" | while read dev; do
    echo "  $dev"
done
