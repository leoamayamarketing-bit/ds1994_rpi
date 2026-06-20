#include "ds1994/ds1994.h"

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

void print_banner() {
    std::cout << "\n"
"╔════════════════════════════════════════════════════════════════╗\n"
"║            DS1994-F5 iButton - Framework v2.1                  ║\n"
"║        Maxim 1-Wire RTC + SRAM Driver for Raspberry Pi        ║\n"
"╚════════════════════════════════════════════════════════════════╝\n";
}

void print_usage(const char* prog) {
    std::cout << "\nUsage: " << prog << " [command] [args...]\n"
"\nCommands:\n"
"  info                         Display full device info\n"
"  scan                         Scan for DS1994 devices\n"
"  status                       Read Status & Control registers\n"
"  dump                         Hex dump of memory (SRAM + timekeeping)\n"
"  rtcdump                      Dump timekeeping registers (0x0200-0x021D)\n"
"  rtc                          Read RTC\n"
"  setrtc                       Sync RTC with system clock\n"
"  interval                     Read Interval Timer\n"
"  cycles                       Read Cycle Counter\n"
"  read <page>                  Read a 32-byte page (0-15)\n"
"  write <page> <hex>           Write hex data to a page\n"
"  osc on|off                   Enable/disable oscillator\n"
"  test                         Write test (0xAA @ 0x0000)\n"
"  wp                           Check write protection status\n"
"  help                         Show this help\n"
"\nExamples:\n"
"  sudo " << prog << " info\n"
"  sudo " << prog << " read 0\n"
"  sudo " << prog << " write 0 48656C6C6F\n"
"  sudo " << prog << " setrtc\n"
"\n";
}

void print_help_table() {
    std::cout << "\n"
"┌───────────────┬───────────────────────────────────────────────┐\n"
"│ COMMAND       │ DESCRIPTION                                   │\n"
"├───────────────┼───────────────────────────────────────────────┤\n"
"│ info          │ Display full device info                      │\n"
"│ scan          │ Scan for DS1994 devices on the bus            │\n"
"│ status        │ Read Status (0x0200) & Control (0x0201)       │\n"
"│ dump          │ Hex dump memory (SRAM + timekeeping)          │\n"
"│ rtcdump       │ Dump timekeeping registers (0x0200-0x021D)    │\n"
"│ rtc           │ Read RTC (Unix timestamp)                     │\n"
"│ setrtc        │ Synchronize RTC with system clock             │\n"
"│ interval      │ Read Interval Timer (seconds + 1/256)         │\n"
"│ cycles        │ Read Cycle Counter (power-on cycles)          │\n"
"│ read <page>   │ Read a 32-byte page (0-15)                    │\n"
"│ write <p> <h> │ Write hex data to page                        │\n"
"│ osc on/off    │ Control internal oscillator                   │\n"
"│ test          │ Write 0xAA to 0x0000 and verify               │\n"
"│ wp            │ Check write protection status                 │\n"
"│ help          │ Show this help menu                           │\n"
"└───────────────┴───────────────────────────────────────────────┘\n";
}

std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    std::vector<uint8_t> data;
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        data.push_back(static_cast<uint8_t>(
            std::stoi(hex.substr(i, 2), nullptr, 16)));
    }
    return data;
}

}  // anonymous namespace

int main(int argc, char* argv[]) {
    print_banner();

    std::string bus_path = "/sys/devices/w1_bus_master1";
    if (const char* env = std::getenv("DS1994_BUS_PATH")) {
        bus_path = env;
    }

    ds1994::DS1994 ds1994(nullptr, bus_path);

    if (!ds1994.select_device()) {
        std::cerr << "\n[!] No DS1994 device found on: " << bus_path << "\n"
                  << "    Check: 1) ls " << bus_path << "/\n"
                  << "           2) Physical connection & 4.7k\u03A9 pull-up\n"
                  << "           3) sudo modprobe w1-gpio w1-therm\n";
        return 1;
    }

    std::cout << "\n[*] Device: " << ds1994.current_device_id() << "\n";

    std::string cmd = (argc > 1) ? argv[1] : "info";

    try {
        if (cmd == "info") {
            ds1994.print_info();
        }
        else if (cmd == "scan") {
            auto devices = ds1994.scan_devices();
            std::cout << "\nDevices found: " << devices.size() << "\n";
            for (const auto& dev : devices) {
                std::cout << "  [" << (dev.active ? "ACTIVE" : "INACTIVE")
                          << "] " << dev.id << "\n";
            }
        }
        else if (cmd == "status") {
            auto status = ds1994.read_status();
            auto control = ds1994.read_control();
            std::cout << "\nStatus Register:  0x"
                      << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(status.to_byte()) << std::dec << "\n"
                      << "Control Register: 0x"
                      << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(control.to_byte()) << std::dec << "\n"
                      << "\nControl bits:"
                      << "  WPR=" << control.wpr
                      << "  WPI=" << control.wpi
                      << "  WPC=" << control.wpc
                      << "  RO="  << control.ro
                      << "  OSC=" << control.osc
                      << "  AUTO=" << control.auto_mode
                      << "  STP=" << control.stop
                      << "  DSEL=" << control.dsel << "\n";
        }
        else if (cmd == "dump") {
            ds1994.dump_memory(true);
        }
        else if (cmd == "rtcdump") {
            auto data = ds1994.read_memory(0x0200, 30);
            std::cout << "\nTimekeeping Registers (0x0200 - 0x021D):\n";
            for (size_t i = 0; i < data.size(); ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(data[i]) << " ";
                if ((i + 1) % 16 == 0) std::cout << "\n";
            }
            std::cout << std::dec << "\n";
        }
        else if (cmd == "rtc") {
            time_t t = ds1994.read_rtc();
            std::cout << "\nRTC: " << std::ctime(&t);
        }
        else if (cmd == "setrtc") {
            time_t now = std::time(nullptr);
            if (ds1994.write_rtc(now)) {
                std::cout << "\nRTC updated: " << std::ctime(&now);
            } else {
                std::cerr << "\nFailed to write RTC (device may be write-protected)\n";
            }
        }
        else if (cmd == "interval") {
            auto val = ds1994.read_interval_timer();
            std::cout << "\nInterval Timer: " << std::fixed
                      << std::setprecision(3) << val.to_seconds()
                      << " sec\n";
        }
        else if (cmd == "cycles") {
            std::cout << "\nCycle Counter: " << ds1994.read_cycle_counter()
                      << "\n";
        }
        else if (cmd == "read" && argc > 2) {
            int page = std::stoi(argv[2]);
            auto data = ds1994.read_page(page);
            std::cout << "\nPage " << page << " (" << data.size() << " bytes):\n";
            for (size_t i = 0; i < data.size(); ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(data[i]) << " ";
                if ((i + 1) % 16 == 0) std::cout << "\n";
            }
            std::cout << std::dec << "\n";
        }
        else if (cmd == "write" && argc > 3) {
            int page = std::stoi(argv[2]);
            auto data = hex_to_bytes(argv[3]);
            if (ds1994.write_page(page, data)) {
                std::cout << "\nWrite successful\n";
                auto verify = ds1994.read_page(page);
                std::cout << "Verification:\n";
                for (size_t i = 0; i < verify.size(); ++i) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0')
                              << static_cast<int>(verify[i]) << " ";
                    if ((i + 1) % 16 == 0) std::cout << "\n";
                }
                std::cout << std::dec << "\n";
            } else {
                std::cerr << "\nWrite failed\n";
            }
        }
        else if (cmd == "osc" && argc > 2) {
            bool on = (std::strcmp(argv[2], "on") == 0);
            if (ds1994.set_oscillator(on)) {
                std::cout << "\nOscillator " << (on ? "ON" : "OFF") << "\n";
            }
        }
        else if (cmd == "test") {
            auto before = ds1994.read_memory(0x0000, 1);
            uint8_t orig = before.empty() ? 0xFF : before[0];
            std::cout << "\nOriginal value at 0x0000: 0x"
                      << std::hex << static_cast<int>(orig) << std::dec << "\n";

            if (ds1994.test_write()) {
                std::cout << "TEST PASSED: Device write works\n";
            } else {
                std::cout << "TEST FAILED: Write-protected or error\n";
            }
        }
        else if (cmd == "wp") {
            bool wp = ds1994.is_write_protected();
            auto ctrl = ds1994.read_control();
            std::cout << "\nWrite Protection Status:\n"
                      << "  RO  (Read Only):  " << (ctrl.ro  ? "YES" : "no") << "\n"
                      << "  WPR (RTC):        " << (ctrl.wpr ? "YES" : "no") << "\n"
                      << "  WPI (Interval):   " << (ctrl.wpi ? "YES" : "no") << "\n"
                      << "  WPC (Cycle):      " << (ctrl.wpc ? "YES" : "no") << "\n"
                      << "  Overall:          " << (wp ? "PROTECTED" : "UNLOCKED") << "\n";
        }
        else {
            print_usage(argv[0]);
            print_help_table();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
