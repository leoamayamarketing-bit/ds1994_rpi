# DS1994-F5 iButton Driver para Raspberry Pi

Driver y CLI en C++17 para el **Maxim DS1994-F5 iButton** (RTC + 512B SRAM + Interval Timer + Cycle Counter sobre bus 1-Wire).

## Compilar

```bash
cd ds1994_app
make
```

## Ejecutar

```bash
# Cargar módulos del kernel (1-Wire en GPIO4)
sudo modprobe w1-gpio w1-therm

# Información completa del dispositivo
sudo ./bin/ds1994_app info

# Leer RTC
sudo ./bin/ds1994_app rtc

# Sincronizar RTC con hora del sistema
sudo ./bin/ds1994_app setrtc

# Volcado hex de toda la memoria
sudo ./bin/ds1994_app dump

# Leer página 0
sudo ./bin/ds1994_app read 0

# Escribir "Hello" en página 0
sudo ./bin/ds1994_app write 0 48656C6C6F

# Verificar protección de escritura
sudo ./bin/ds1994_app wp

# Prueba de escritura
sudo ./bin/ds1994_app test
```

## Comandos

| Comando | Descripción |
|---------|-------------|
| `info` | Información completa del dispositivo |
| `scan` | Escanear bus 1-Wire |
| `status` | Leer Status y Control registers |
| `dump` | Volcado hex completo (542 bytes) |
| `ramdump` | Solo SRAM (512 bytes) |
| `rtcdump` | Solo registros timekeeping |
| `rtc` | Leer RTC |
| `setrtc` | Escribir hora del sistema en RTC |
| `interval` | Leer Interval Timer |
| `cycles` | Leer Cycle Counter |
| `read <pag>` | Leer página (0-15) |
| `write <pag> <hex>` | Escribir datos hex en página |
| `osc on/off` | Controlar oscilador |
| `test` | Prueba de escritura |
| `wp` | Estado de protección |
| `help` | Mostrar ayuda |
