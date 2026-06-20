#ifndef DS1994_COMMANDS_H
#define DS1994_COMMANDS_H

#include <cstdint>

namespace ds1994 {

constexpr uint8_t CMD_READ_ROM         = 0x33;
constexpr uint8_t CMD_SKIP_ROM         = 0xCC;
constexpr uint8_t CMD_WRITE_SCRATCHPAD = 0x0F;
constexpr uint8_t CMD_READ_SCRATCHPAD  = 0xAA;
constexpr uint8_t CMD_COPY_SCRATCHPAD  = 0x55;
constexpr uint8_t CMD_READ_MEMORY      = 0xF0;

constexpr uint16_t ADDR_STATUS   = 0x0200;
constexpr uint16_t ADDR_CONTROL  = 0x0201;
constexpr uint16_t ADDR_RTC      = 0x0202;
constexpr uint16_t ADDR_INTERVAL = 0x0207;
constexpr uint16_t ADDR_CYCLE    = 0x020C;

constexpr int PAGE_SIZE  = 32;
constexpr int NUM_PAGES  = 16;
constexpr int TOTAL_SRAM = 512;
constexpr int TOTAL_MEM  = 542;

constexpr uint8_t CR_WPR  = 0x01;
constexpr uint8_t CR_WPI  = 0x02;
constexpr uint8_t CR_WPC  = 0x04;
constexpr uint8_t CR_RO   = 0x08;
constexpr uint8_t CR_OSC  = 0x10;
constexpr uint8_t CR_AUTO = 0x20;
constexpr uint8_t CR_STOP = 0x40;
constexpr uint8_t CR_DSEL = 0x80;

constexpr uint8_t SR_RTF  = 0x01;
constexpr uint8_t SR_ITF  = 0x02;
constexpr uint8_t SR_CCF  = 0x04;
constexpr uint8_t SR_RTE  = 0x08;
constexpr uint8_t SR_ITE  = 0x10;
constexpr uint8_t SR_CCE  = 0x20;

}  // namespace ds1994

#endif
