#ifndef DS1994_DRIVER_H
#define DS1994_DRIVER_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

#include "types.h"
#include "commands.h"

namespace ds1994 {

class DS1994 {
public:
    explicit DS1994(EventCallback callback = nullptr,
                    const std::string& bus_path = "/sys/devices/w1_bus_master1");
    ~DS1994();

    DS1994(const DS1994&) = delete;
    DS1994& operator=(const DS1994&) = delete;
    DS1994(DS1994&&) noexcept;
    DS1994& operator=(DS1994&&) noexcept;

    std::vector<DeviceInfo> scan_devices();
    bool select_device(const std::string& device_id = "");
    std::string current_device_id() const;

    std::vector<uint8_t> read_memory(uint16_t address, size_t length);
    bool write_memory(uint16_t address, const std::vector<uint8_t>& data);
    std::vector<uint8_t> read_page(int page);
    bool write_page(int page, const std::vector<uint8_t>& data, int offset = 0);

    StatusRegister read_status();
    ControlRegister read_control();
    time_t read_rtc();
    bool write_rtc(time_t unix_time);
    IntervalTimer read_interval_timer();
    uint32_t read_cycle_counter();
    bool set_oscillator(bool enable);

    bool is_write_protected();
    bool test_write();

    void dump_memory(bool include_timekeeping = true);
    void print_info();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}  // namespace ds1994

#endif
