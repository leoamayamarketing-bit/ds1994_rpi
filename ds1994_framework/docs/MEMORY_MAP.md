# DS1994-F5 Memory Map

## Address Layout

| Address Range | Size | Region | Type |
|---------------|------|--------|------|
| `0x0000` - `0x01FF` | 512 B | User SRAM (16 pages x 32 B) | Read/Write |
| `0x0200` | 1 B | Status Register | Read only |
| `0x0201` | 1 B | Control Register | Read/Write (OTP bits) |
| `0x0202` - `0x0206` | 5 B | RTC | Read/Write |
| `0x0207` - `0x020B` | 5 B | Interval Timer | Read only |
| `0x020C` - `0x020F` | 4 B | Cycle Counter | Read/Write* |
| `0x0210` - `0x021D` | 14 B | Reserved | - |

\* Cycle Counter is read-only if WPC bit is set.

## Status Register (`0x0200`)

| Bit | Name | Description |
|-----|------|-------------|
| 0 | RTF | RTC Alarm Flag |
| 1 | ITF | Interval Timer Alarm Flag |
| 2 | CCF | Cycle Counter Alarm Flag |
| 3 | RTE | RTC Interrupt Enable |
| 4 | ITE | Interval Timer Interrupt Enable |
| 5 | CCE | Cycle Counter Interrupt Enable |
| 6-7 | - | Reserved |

## Control Register (`0x0201`)

| Bit | Name | Description | OTP |
|-----|------|-------------|-----|
| 0 | WPR | RTC Write Protect | Yes |
| 1 | WPI | Interval Timer Write Protect | Yes |
| 2 | WPC | Cycle Counter Write Protect | Yes |
| 3 | RO | Read-Only Mode (disables all writes) | Yes |
| 4 | OSC | Oscillator Enable | No |
| 5 | AUTO | Auto Mode (vs Manual) | No |
| 6 | STOP | Timer Stop (in Manual mode) | No |
| 7 | DSEL | Delay Select (3.5ms / 123ms) | No |

Once an OTP bit is set to 1, it **cannot be cleared**. If `RO=1` the device is permanently read-only.

## RTC (`0x0202`)

5 bytes: `[fraction, seconds_lo, seconds_mid, seconds_hi, seconds_ex]`

- Byte 0: Fractional seconds (1/256ths)
- Bytes 1-4: 32-bit Unix timestamp (little-endian)

## Interval Timer (`0x0207`)

5 bytes: `[fraction, seconds_lo, seconds_mid, seconds_hi, seconds_ex]`

- Byte 0: Fractional seconds (1/256ths)
- Bytes 1-4: 32-bit seconds counter (little-endian)

## Cycle Counter (`0x020C`)

4 bytes: 32-bit little-endian counter of power-on or interval cycles.

## 1-Wire Commands

| Command | Byte | Description |
|---------|------|-------------|
| READ_ROM | `0x33` | Read 64-bit ROM |
| SKIP_ROM | `0xCC` | Skip ROM (not used by this driver) |
| WRITE_SCRATCHPAD | `0x0F` | Write data to 32-byte scratchpad |
| READ_SCRATCHPAD | `0xAA` | Read scratchpad + ending offset |
| COPY_SCRATCHPAD | `0x55` | Copy scratchpad to target address |
| READ_MEMORY | `0xF0` | Read memory from target address |

### Write Sequence (3-Step Scratchpad Protocol)

1. **WRITE_SCRATCHPAD** (`0x0F`) + TA1 + TA2 + data -- loads data into scratchpad
2. **READ_SCRATCHPAD** (`0xAA`) -- reads back TA1, TA2, E/S for verification
3. **COPY_SCRATCHPAD** (`0x55`) + TA1 + TA2 + E/S -- commits to memory (30 µs)
