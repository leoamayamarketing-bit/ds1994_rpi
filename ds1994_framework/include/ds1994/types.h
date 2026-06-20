#ifndef DS1994_TYPES_H
#define DS1994_TYPES_H

#include <string>
#include <vector>
#include <cstdint>
#include <functional>

#include "commands.h"

namespace ds1994 {

struct DeviceInfo {
    std::string id;
    std::string path;
    uint8_t family;
    std::string serial;
    bool active;
};

struct ControlRegister {
    bool wpr;
    bool wpi;
    bool wpc;
    bool ro;
    bool osc;
    bool auto_mode;
    bool stop;
    bool dsel;

    static ControlRegister from_byte(uint8_t value) {
        return {
            (value & CR_WPR) != 0,
            (value & CR_WPI) != 0,
            (value & CR_WPC) != 0,
            (value & CR_RO)  != 0,
            (value & CR_OSC) != 0,
            (value & CR_AUTO) != 0,
            (value & CR_STOP) != 0,
            (value & CR_DSEL) != 0,
        };
    }

    uint8_t to_byte() const {
        return (wpr  ? CR_WPR  : 0) |
               (wpi  ? CR_WPI  : 0) |
               (wpc  ? CR_WPC  : 0) |
               (ro   ? CR_RO   : 0) |
               (osc  ? CR_OSC  : 0) |
               (auto_mode ? CR_AUTO : 0) |
               (stop ? CR_STOP : 0) |
               (dsel ? CR_DSEL : 0);
    }
};

struct StatusRegister {
    bool rtf;
    bool itf;
    bool ccf;
    bool rte;
    bool ite;
    bool cce;

    static StatusRegister from_byte(uint8_t value) {
        return {
            (value & SR_RTF) != 0,
            (value & SR_ITF) != 0,
            (value & SR_CCF) != 0,
            (value & SR_RTE) != 0,
            (value & SR_ITE) != 0,
            (value & SR_CCE) != 0,
        };
    }

    uint8_t to_byte() const {
        return (rtf  ? SR_RTF : 0) |
               (itf  ? SR_ITF : 0) |
               (ccf  ? SR_CCF : 0) |
               (rte  ? SR_RTE : 0) |
               (ite  ? SR_ITE : 0) |
               (cce  ? SR_CCE : 0);
    }
};

struct RTCData {
    uint8_t fraction;
    uint32_t seconds;
};

struct IntervalTimer {
    uint8_t fraction;
    uint32_t seconds;

    double to_seconds() const {
        return seconds + fraction / 256.0;
    }
};

using EventCallback = std::function<void(const std::string& event,
                                        const std::string& message)>;

}  // namespace ds1994

#endif
