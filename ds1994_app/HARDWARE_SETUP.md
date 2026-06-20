# Conexión del DS1994-F5 a Raspberry Pi

## Conexión Directa (GPIO4)

```
Raspberry Pi              DS1994-F5 iButton
─────────────             ────────────────
GPIO4 (pin 7) ─────[4.7kΩ]──── Centro (Data)
GND    (pin 6) ──────────────── Case (GND)
3.3V   (pin 1) ───[4.7kΩ]────┘
```

### Configurar 1-Wire en Raspberry Pi

```bash
# Habilitar en config.txt
echo "dtoverlay=w1-gpio,gpiopin=4" | sudo tee -a /boot/config.txt
sudo reboot

# Cargar módulos
sudo modprobe w1-gpio
sudo modprobe w1-therm
```

## Conexión vía DS2482 (I2C)

```
Pi GPIO2 (SDA) ──[4.7kΩ]── DS2482 SDA
Pi GPIO3 (SCL) ──[4.7kΩ]── DS2482 SCL
Pi 3.3V         ────────── DS2482 VDD
Pi GND          ────────── DS2482 GND
DS2482 1-W I/O  ──[4.7kΩ]── DS1994 Data
```

Ver `scripts/ds2482.sh` para configuración automatizada.
