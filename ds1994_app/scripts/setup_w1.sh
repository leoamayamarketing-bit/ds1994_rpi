#!/bin/bash
# Cargar módulos 1-Wire para Raspberry Pi (GPIO4)
echo "Cargando módulos 1-Wire..."
sudo modprobe w1-gpio && sudo modprobe w1-therm
echo "Módulos cargados."
echo ""
echo "Dispositivos en el bus:"
ls /sys/devices/w1_bus_master1/ 2>/dev/null | grep -E '^[0-9a-f]{2}-' || echo "(ninguno)"
