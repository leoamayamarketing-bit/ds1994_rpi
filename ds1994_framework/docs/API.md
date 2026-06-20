# DS1994 C++ API Reference

Namespace: `ds1994`
Header: `#include "ds1994/ds1994.h"`

## Types (`ds1994/types.h`)

### `DeviceInfo`
```cpp
struct DeviceInfo {
    std::string id;       // e.g. "04-00000050eb7a"
    std::string path;     // e.g. "/sys/devices/w1_bus_master1/04-.../rw"
    uint8_t family;       // 0x04
    std::string serial;   // e.g. "00000050eb7a"
    bool active;          // true if rw file exists
};
```

### `ControlRegister`
```cpp
struct ControlRegister {
    bool wpr, wpi, wpc;  // Write protect bits (OTP)
    bool ro;              // Read-only mode (OTP)
    bool osc;             // Oscillator
    bool auto_mode;       // Auto/Manual mode
    bool stop;            // Timer stop
    bool dsel;            // Delay select
    static ControlRegister from_byte(uint8_t value);
    uint8_t to_byte() const;
};
```

### `StatusRegister`
```cpp
struct StatusRegister {
    bool rtf, itf, ccf;  // Alarm flags
    bool rte, ite, cce;  // Interrupt enables
    static StatusRegister from_byte(uint8_t value);
};
```

### `IntervalTimer`
```cpp
struct IntervalTimer {
    uint8_t fraction;     // 1/256ths of a second
    uint32_t seconds;
    double to_seconds() const;
};
```

## Class `DS1994`

### Construction

```cpp
explicit DS1994(EventCallback callback = nullptr,
                const std::string& bus_path = "/sys/devices/w1_bus_master1");
```

- `callback`: optional event notification function
- `bus_path`: custom sysfs path (overridable via `DS1994_BUS_PATH` env var)

### Device Discovery

```cpp
std::vector<DeviceInfo> scan_devices();
bool select_device(const std::string& device_id = "");
std::string current_device_id() const;
```

- `select_device("")` auto-selects the first DS1994 found.

### Memory Access

```cpp
std::vector<uint8_t> read_memory(uint16_t address, size_t length);
bool write_memory(uint16_t address, const std::vector<uint8_t>& data);
std::vector<uint8_t> read_page(int page);
bool write_page(int page, const std::vector<uint8_t>& data, int offset = 0);
```

- `page`: 0-15 (32 bytes each)
- `data.size()` must be 1-32 bytes for `write_memory()`

### Timekeeping

```cpp
StatusRegister read_status();
ControlRegister read_control();
time_t read_rtc();
bool write_rtc(time_t unix_time);
IntervalTimer read_interval_timer();
uint32_t read_cycle_counter();
bool set_oscillator(bool enable);
```

### Diagnostics

```cpp
bool is_write_protected();
bool test_write();
void dump_memory(bool include_timekeeping = true);
void print_info();
```
