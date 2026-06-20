#include "ds1994/ds1994.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>

namespace fs = std::filesystem;

namespace ds1994 {

class DS1994::Impl {
public:
    Impl(EventCallback callback, const std::string& bus_path)
        : m_bus_path(bus_path), m_callback(std::move(callback)),
          m_initialized(false) {}

    std::vector<DeviceInfo> scan_devices() {
        std::vector<DeviceInfo> devices;
        if (!fs::exists(m_bus_path)) {
            notify("SCAN", "Bus not found: " + m_bus_path);
            return devices;
        }

        DIR* dir = opendir(m_bus_path.c_str());
        if (!dir) return devices;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name(entry->d_name);
            if (name.length() < 15 || name[2] != '-') continue;

            std::string family = name.substr(0, 2);
            if (family != "04") continue;

            DeviceInfo dev;
            dev.id = name;
            dev.path = m_bus_path + "/" + name + "/rw";
            dev.family = static_cast<uint8_t>(std::stoi(family, nullptr, 16));
            dev.serial = name.substr(3);
            dev.active = fs::exists(dev.path);

            if (dev.active) {
                devices.push_back(std::move(dev));
                notify("SCAN", "Found device: " + name);
            }
        }
        closedir(dir);
        return devices;
    }

    bool select_device(const std::string& device_id) {
        if (!device_id.empty()) {
            m_device_id = device_id;
            m_device_path = m_bus_path + "/" + m_device_id + "/rw";
        } else {
            auto devices = scan_devices();
            if (devices.empty()) return false;
            m_device_id = devices[0].id;
            m_device_path = devices[0].path;
        }

        if (!fs::exists(m_device_path)) return false;
        m_initialized = true;
        return true;
    }

    std::string current_device_id() const {
        return m_device_id;
    }

    std::vector<uint8_t> read_memory(uint16_t address, size_t length) {
        if (!m_initialized) return {};

        std::vector<uint8_t> cmd = {
            CMD_READ_MEMORY,
            static_cast<uint8_t>(address & 0xFF),
            static_cast<uint8_t>((address >> 8) & 0xFF)
        };

        if (!write_raw(cmd)) return {};
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return read_raw(length);
    }

    bool write_memory(uint16_t address, const std::vector<uint8_t>& data) {
        if (!m_initialized || data.empty() || data.size() > 32) return false;

        uint8_t ta1 = static_cast<uint8_t>(address & 0xFF);
        uint8_t ta2 = static_cast<uint8_t>((address >> 8) & 0xFF);

        std::vector<uint8_t> write_cmd = {CMD_WRITE_SCRATCHPAD, ta1, ta2};
        write_cmd.insert(write_cmd.end(), data.begin(), data.end());

        if (!write_raw(write_cmd)) return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (!write_raw({CMD_READ_SCRATCHPAD})) return false;
        auto scratch = read_raw(3);

        uint8_t ending_offset = static_cast<uint8_t>((ta1 & 0x1F) + data.size() - 1);
        std::vector<uint8_t> copy_cmd = {
            CMD_COPY_SCRATCHPAD, ta1, ta2,
            static_cast<uint8_t>(ending_offset & 0x1F)
        };

        if (!write_raw(copy_cmd)) return false;
        std::this_thread::sleep_for(std::chrono::microseconds(30));
        return true;
    }

    std::vector<uint8_t> read_page(int page) {
        if (page < 0 || page >= NUM_PAGES) return {};
        return read_memory(static_cast<uint16_t>(page * PAGE_SIZE), PAGE_SIZE);
    }

    bool write_page(int page, const std::vector<uint8_t>& data, int offset) {
        if (page < 0 || page >= NUM_PAGES) return false;
        if (offset + static_cast<int>(data.size()) > PAGE_SIZE) return false;
        return write_memory(
            static_cast<uint16_t>(page * PAGE_SIZE + offset), data);
    }

    StatusRegister read_status() {
        auto data = read_memory(ADDR_STATUS, 1);
        return StatusRegister::from_byte(data.empty() ? 0xFF : data[0]);
    }

    ControlRegister read_control() {
        auto data = read_memory(ADDR_CONTROL, 1);
        return ControlRegister::from_byte(data.empty() ? 0xFF : data[0]);
    }

    time_t read_rtc() {
        auto data = read_memory(ADDR_RTC, 5);
        if (data.size() < 5) return 0;
        uint32_t seconds = (static_cast<uint32_t>(data[4]) << 24) |
                           (static_cast<uint32_t>(data[3]) << 16) |
                           (static_cast<uint32_t>(data[2]) << 8)  |
                           data[1];
        return static_cast<time_t>(seconds);
    }

    bool write_rtc(time_t unix_time) {
        std::vector<uint8_t> data(5, 0);
        data[1] = static_cast<uint8_t>(unix_time & 0xFF);
        data[2] = static_cast<uint8_t>((unix_time >> 8) & 0xFF);
        data[3] = static_cast<uint8_t>((unix_time >> 16) & 0xFF);
        data[4] = static_cast<uint8_t>((unix_time >> 24) & 0xFF);
        return write_memory(ADDR_RTC, data);
    }

    IntervalTimer read_interval_timer() {
        auto data = read_memory(ADDR_INTERVAL, 5);
        if (data.size() < 5) return {0, 0};
        return {data[0],
                (static_cast<uint32_t>(data[4]) << 24) |
                (static_cast<uint32_t>(data[3]) << 16) |
                (static_cast<uint32_t>(data[2]) << 8)  |
                data[1]};
    }

    uint32_t read_cycle_counter() {
        auto data = read_memory(ADDR_CYCLE, 4);
        if (data.size() < 4) return 0;
        return (static_cast<uint32_t>(data[3]) << 24) |
               (static_cast<uint32_t>(data[2]) << 16) |
               (static_cast<uint32_t>(data[1]) << 8)  |
               data[0];
    }

    bool set_oscillator(bool enable) {
        auto control = read_control();
        uint8_t value = control.to_byte();
        if (enable) value |= CR_OSC;
        else value &= ~CR_OSC;
        return write_memory(ADDR_CONTROL, {value});
    }

    bool is_write_protected() {
        auto ctrl = read_control();
        return ctrl.ro || ctrl.wpr || ctrl.wpi || ctrl.wpc;
    }

    bool test_write() {
        auto before = read_memory(0x0000, 1);

        if (!write_memory(0x0000, {0xAA})) return false;

        auto after = read_memory(0x0000, 1);
        uint8_t result = after.empty() ? 0xFF : after[0];
        return result == 0xAA;
    }

    void dump_memory(bool include_timekeeping) {
        size_t total = include_timekeeping ? TOTAL_MEM : TOTAL_SRAM;
        auto mem = read_memory(0x0000, total);

        for (size_t i = 0; i < mem.size(); i += 16) {
            std::cout << std::hex << std::setw(4) << std::setfill('0')
                      << i << ": ";
            for (size_t j = i; j < std::min(i + 16, mem.size()); ++j) {
                std::cout << std::setw(2) << std::setfill('0')
                          << static_cast<int>(mem[j]) << " ";
            }
            std::cout << " |";
            for (size_t j = i; j < std::min(i + 16, mem.size()); ++j) {
                char c = (mem[j] >= 32 && mem[j] <= 126)
                             ? static_cast<char>(mem[j]) : '.';
                std::cout << c;
            }
            std::cout << "|\n";
        }
        std::cout << std::dec;
    }

    void print_info() {
        auto devs = scan_devices();
        bool found = false;
        for (const auto& d : devs) {
            if (d.id == m_device_id) { found = true; break; }
        }
        if (!found) {
            std::cout << "\n[!] Device " << m_device_id << " not currently visible on bus.\n";
            return;
        }

        std::cout << "\n========================================\n"
                  << "   DS1994-F5 iButton - Device Info\n"
                  << "========================================\n"
                  << " Device ID:   " << m_device_id << "\n"
                  << " Bus Path:    " << m_bus_path << "\n"
                  << " Device Path: " << m_device_path << "\n"
                  << "========================================\n";

        auto status = read_status();
        auto control = read_control();

        std::cout << "\n Status Register:  0x"
                  << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(status.to_byte()) << std::dec;
        if (status.to_byte() == 0) std::cout << " (no alarms)";
        std::cout << "\n Control Register: 0x"
                  << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(control.to_byte()) << std::dec << "\n";

        std::cout << "\n Control Bits:\n"
                  << "   WPR=" << control.wpr
                  << "  WPI=" << control.wpi
                  << "  WPC=" << control.wpc
                  << "  RO="  << control.ro
                  << "  OSC=" << control.osc
                  << "  AUTO=" << control.auto_mode
                  << "  STP=" << control.stop
                  << "  DSEL=" << control.dsel << "\n";

        time_t t = read_rtc();
        char time_buf[64];
        if (ctime_r(&t, time_buf)) {
            time_buf[24] = '\0';
            std::cout << "\n RTC: " << time_buf << "\n";
        }

        auto interval = read_interval_timer();
        std::cout << " Interval Timer: " << std::fixed
                  << std::setprecision(3) << interval.to_seconds()
                  << " sec\n"
                  << " Cycle Counter: " << read_cycle_counter()
                  << " cycles\n"
                  << "========================================\n";
    }

private:
    std::string m_bus_path;
    std::string m_device_id;
    std::string m_device_path;
    EventCallback m_callback;
    bool m_initialized;

    bool write_raw(const std::vector<uint8_t>& data) {
        if (m_device_path.empty()) return false;
        std::ofstream dev(m_device_path, std::ios::binary);
        if (!dev.is_open()) return false;
        dev.write(reinterpret_cast<const char*>(data.data()),
                  static_cast<std::streamsize>(data.size()));
        dev.flush();
        return dev.good();
    }

    std::vector<uint8_t> read_raw(size_t num_bytes) {
        std::ifstream dev(m_device_path, std::ios::binary);
        if (!dev.is_open()) return {};
        std::vector<uint8_t> buffer(num_bytes);
        dev.read(reinterpret_cast<char*>(buffer.data()),
                 static_cast<std::streamsize>(num_bytes));
        buffer.resize(static_cast<size_t>(dev.gcount()));
        return buffer;
    }

    void notify(const std::string& event, const std::string& message) {
        if (m_callback) m_callback(event, message);
    }
};

DS1994::DS1994(EventCallback callback, const std::string& bus_path)
    : pImpl(std::make_unique<Impl>(std::move(callback), bus_path)) {}

DS1994::~DS1994() = default;

DS1994::DS1994(DS1994&&) noexcept = default;
DS1994& DS1994::operator=(DS1994&&) noexcept = default;

std::vector<DeviceInfo> DS1994::scan_devices() { return pImpl->scan_devices(); }
bool DS1994::select_device(const std::string& device_id) { return pImpl->select_device(device_id); }
std::string DS1994::current_device_id() const { return pImpl->current_device_id(); }
std::vector<uint8_t> DS1994::read_memory(uint16_t address, size_t length) { return pImpl->read_memory(address, length); }
bool DS1994::write_memory(uint16_t address, const std::vector<uint8_t>& data) { return pImpl->write_memory(address, data); }
std::vector<uint8_t> DS1994::read_page(int page) { return pImpl->read_page(page); }
bool DS1994::write_page(int page, const std::vector<uint8_t>& data, int offset) { return pImpl->write_page(page, data, offset); }
StatusRegister DS1994::read_status() { return pImpl->read_status(); }
ControlRegister DS1994::read_control() { return pImpl->read_control(); }
time_t DS1994::read_rtc() { return pImpl->read_rtc(); }
bool DS1994::write_rtc(time_t unix_time) { return pImpl->write_rtc(unix_time); }
IntervalTimer DS1994::read_interval_timer() { return pImpl->read_interval_timer(); }
uint32_t DS1994::read_cycle_counter() { return pImpl->read_cycle_counter(); }
bool DS1994::set_oscillator(bool enable) { return pImpl->set_oscillator(enable); }
bool DS1994::is_write_protected() { return pImpl->is_write_protected(); }
bool DS1994::test_write() { return pImpl->test_write(); }
void DS1994::dump_memory(bool include_timekeeping) { pImpl->dump_memory(include_timekeeping); }
void DS1994::print_info() { pImpl->print_info(); }

}  // namespace ds1994
