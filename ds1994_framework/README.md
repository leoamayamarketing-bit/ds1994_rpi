# DS1994-F5 Framework

**C++17 driver and CLI utility for the Maxim DS1994-F5 iButton** -- a 1-Wire device combining Real-Time Clock (RTC), Interval Timer, Cycle Counter, and 512 bytes of SRAM, designed for **Raspberry Pi**.

## Overview

This project is the result of analyzing three existing DS1994 projects:

| Project | Type | Contribution |
|---------|------|-------------|
| `prj_ds1994_deepseek_v2/` | C++ (multi-file, PIMPL) | Architecture, PIMPL idiom, Makefile build |
| `src_deepseek/` | C++ (single-file) | Core driver logic, scratchpad protocol, CLI |
| `scrpits_ds1994/` | Bash scripts | Bus diagnostics, GPIO setup, DS2482 detection |

This framework unifies the best of all three: the PIMPL architecture, comprehensive CLI, CMake build system, shell tools, and full documentation.

## Features

- Automatic DS1994 device detection via sysfs (`/sys/devices/w1_bus_master1/`)
- SRAM read/write (byte, page, full dump) with 3-step scratchpad protocol
- RTC read/synchronization (32-bit Unix timestamp)
- Interval Timer read (seconds + 1/256 fractional)
- Cycle Counter read (32-bit power-on cycles)
- Oscillator enable/disable
- Write protection diagnostics
- Configurable bus path via `DS1994_BUS_PATH` env var
- Structured types for `ControlRegister`, `StatusRegister`, `IntervalTimer`
- Event callback support

## Build

```bash
mkdir build && cd build
cmake ..
make
sudo ./bin/ds1994_app info
```

Dependencies: `g++` (C++17), CMake 3.14+, pthreads, Linux with `w1-gpio` + `w1-therm` modules.

## Quick Start

```bash
# Load 1-Wire kernel modules
sudo modprobe w1-gpio w1-therm

# Scan for devices
sudo ./bin/ds1994_app scan

# Read device info
sudo ./bin/ds1994_app info

# Read RTC
sudo ./bin/ds1994_app rtc

# Sync RTC with system clock
sudo ./bin/ds1994_app setrtc

# Hex dump of full memory
sudo ./bin/ds1994_app dump

# Read/write a page
sudo ./bin/ds1994_app read 0
sudo ./bin/ds1994_app write 0 48656C6C6F

# Check write protection
sudo ./bin/ds1994_app wp
```

## Commands

| Command | Description |
|---------|-------------|
| `info` | Full device info (registers, RTC, timer, cycles) |
| `scan` | Scan bus for DS1994 devices |
| `status` | Read Status & Control registers |
| `dump` | Hex dump of SRAM + timekeeping registers |
| `rtcdump` | Dump timekeeping registers only |
| `rtc` | Read RTC as human-readable time |
| `setrtc` | Write system time to DS1994 RTC |
| `interval` | Read Interval Timer |
| `cycles` | Read Cycle Counter |
| `read <page>` | Read 32-byte page (0-15) |
| `write <page> <hex>` | Write hex data to page |
| `osc on/off` | Control oscillator |
| `test` | Write 0xAA to 0x0000 and verify |
| `wp` | Write protection status |

## License

MIT
