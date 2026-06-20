#!/bin/bash
# Load kernel modules for 1-Wire bus on GPIO4 (Raspberry Pi)
sudo modprobe w1-gpio && sudo modprobe w1-therm
echo "1-Wire modules loaded."
echo "Devices should appear under /sys/devices/w1_bus_master1/"
ls /sys/devices/w1_bus_master1/ 2>/dev/null | grep -E '^[0-9a-f]{2}-' || echo "(no devices detected)"
