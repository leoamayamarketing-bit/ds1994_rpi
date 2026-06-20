#include "ds1994/ds1994.h"
#include <iostream>
#include <iomanip>

int main() {
    ds1994::DS1994 ds1994;

    auto devices = ds1994.scan_devices();
    if (devices.empty()) {
        std::cerr << "No DS1994 devices found.\n";
        return 1;
    }

    if (!ds1994.select_device(devices[0].id)) {
        std::cerr << "Failed to select device.\n";
        return 1;
    }

    std::cout << "Device: " << ds1994.current_device_id() << "\n";

    auto status = ds1994.read_status();
    std::cout << "Status: 0x" << std::hex
              << static_cast<int>(status.to_byte()) << std::dec << "\n";

    auto control = ds1994.read_control();
    std::cout << "Control: 0x" << std::hex
              << static_cast<int>(control.to_byte()) << std::dec << "\n";

    time_t rtc = ds1994.read_rtc();
    std::cout << "RTC: " << std::ctime(&rtc);

    auto interval = ds1994.read_interval_timer();
    std::cout << "Interval: " << interval.to_seconds() << "s\n";

    uint32_t cycles = ds1994.read_cycle_counter();
    std::cout << "Cycles: " << cycles << "\n";

    auto page0 = ds1994.read_page(0);
    std::cout << "Page 0: ";
    for (uint8_t b : page0) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << "\n";

    return 0;
}
