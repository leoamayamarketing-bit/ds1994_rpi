# Mapa de Memoria DS1994-F5

| Dirección | Tamaño | Región | Tipo |
|-----------|--------|--------|------|
| `0x0000` - `0x01FF` | 512 B | SRAM usuario (16 páginas × 32 B) | L/E |
| `0x0200` | 1 B | Status Register | Sólo lectura |
| `0x0201` | 1 B | Control Register | L/E (bits OTP) |
| `0x0202` - `0x0206` | 5 B | RTC (fracción + segundos Unix) | L/E |
| `0x0207` - `0x020B` | 5 B | Interval Timer | Sólo lectura |
| `0x020C` - `0x020F` | 4 B | Cycle Counter | L/E* |
| `0x0210` - `0x021D` | 14 B | Reservado | - |

\* Sólo escritura si WPC = 0.

## Status Register (0x0200)

| Bit | Nombre | Descripción |
|-----|--------|-------------|
| 0 | RTF | RTC Alarm Flag |
| 1 | ITF | Interval Timer Flag |
| 2 | CCF | Cycle Counter Flag |
| 3 | RTE | RTC Interrupt Enable |
| 4 | ITE | Interval Timer Enable |
| 5 | CCE | Cycle Counter Enable |

## Control Register (0x0201)

| Bit | Nombre | Descripción | OTP |
|-----|--------|-------------|-----|
| 0 | WPR | RTC Write Protect | Sí |
| 1 | WPI | Interval Timer WP | Sí |
| 2 | WPC | Cycle Counter WP | Sí |
| 3 | RO | Read Only (todo) | Sí |
| 4 | OSC | Oscilador | No |
| 5 | AUTO | Modo Automático | No |
| 6 | STOP | Timer Stop | No |
| 7 | DSEL | Delay Select | No |

## Protocolo de Escritura (3 pasos)

1. **WRITE_SCRATCHPAD** (0x0F) + TA1 + TA2 + datos
2. **READ_SCRATCHPAD** (0xAA) → verificar TA1, TA2, E/S
3. **COPY_SCRATCHPAD** (0x55) + TA1 + TA2 + E/S (30 µs)
