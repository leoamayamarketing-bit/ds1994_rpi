# Hardware Setup

## Raspberry Pi + DS1994-F5 (Direct GPIO)

```
Raspberry Pi GPIO4 (BCM) ──[4.7kΩ]── DS1994-F5 Data pin
Raspberry Pi GND         ──────────── DS1994-F5 Ground
```

### Schematic

```
Raspberry Pi 3.3V (pin 1)
     │
     └─[4.7kΩ resistor]──┐
                          │
GPIO4 (BCM, pin 7) ──────┴────────── DS1994 Data (center contact)
                          │
GND (pin 6) ───────────────────────── DS1994 Ground (case)
```

### Configuration

```bash
# Enable 1-Wire on GPIO4
echo "dtoverlay=w1-gpio,gpiopin=4" | sudo tee -a /boot/config.txt

# Reboot
sudo reboot

# After reboot, load kernel modules
sudo modprobe w1-gpio
sudo modprobe w1-therm

# Verify
ls /sys/devices/w1_bus_master1/
```

## Raspberry Pi + DS2482 (I2C Bridge)

```
Raspberry Pi GPIO2 (SDA) ──[4.7kΩ]── DS2482 SDA
Raspberry Pi GPIO3 (SCL) ──[4.7kΩ]── DS2482 SCL
Raspberry Pi 3.3V        ──────────── DS2482 VDD
Raspberry Pi GND         ──────────── DS2482 GND
DS2482 1-Wire I/O ───────[4.7kΩ]──── DS1994-F5 Data
```

See `scripts/ds2482_detect.sh` for automated setup.

## iButton Physical Connection

- **Center contact**: Data (1-Wire)
- **Outer case**: Ground
- Use a DS9092 or similar iButton socket/probe
- Keep wires short (< 50 cm ideal, < 2 m max)

## Verification

```bash
# Check modules are loaded
lsmod | grep w1

# List 1-Wire devices
ls /sys/devices/w1_bus_master1/
# Expected: 04-XXXXXXXXXXXX (DS1994 has family code 04)

# Run framework diagnostics
sudo ./bin/ds1994_app info
```

## Troubleshooting

| Symptom | Likely Cause | Solution |
|---------|-------------|----------|
| No devices found | w1 modules not loaded | `sudo modprobe w1-gpio w1-therm` |
| No devices found | Missing pull-up | Add 4.7kΩ resistor between GPIO4 and 3.3V |
| No devices found | Wrong GPIO | Verify `dtoverlay=w1-gpio,gpiopin=4` in config.txt |
| Intermittent readings | Long wires | Shorten cables or reduce bus speed |
| Write fails | OTP protection | Check RO/WPR/WPI/WPC bits with `wp` command |
| Device shows 0xFF | Bus contention | Check pull-up, remove other 1-Wire devices |
