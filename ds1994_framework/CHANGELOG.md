# Changelog

## [2.1.0] - 2026-06-20

### Added
- New CMake-based project `ds1994_framework` unifying three prior projects
- Structured types: `ControlRegister`, `StatusRegister`, `IntervalTimer`, `DeviceInfo`
- Write protection diagnostic command (`wp`)
- RTC dump command (`rtcdump`) for timekeeping register view
- `DS1994_BUS_PATH` environment variable support for custom bus paths
- Example program (`examples/basic_usage.cpp`)
- Documentation: API reference, memory map, hardware setup guide

### Changed
- Migrated from Makefile to CMake build system
- Namespaced all types under `ds1994::`
- Snake_case method naming for C++ consistency
- Modernized includes and removed `using namespace std`
- Separated command constants into `commands.h`, types into `types.h`

### Fixed
- Thread-safe string handling with `ctime_r`
- Explicit `static_cast` for all integer truncation
- Removed `SKIP_ROM` usage per DS1994 datasheet requirements

## [2.0.0] - 2026-04

### prj_ds1994_deepseek_v2 (v3/fixed)
- Added configurable bus path parameter to constructor
- Cleaned up callback usage
- Improved help display with table formatting

## [1.0.0] - 2026-04

### src_deepseek (single-file driver)
- Initial single-file C++17 DS1994 driver
- Full command set: info, dump, status, rtc, setrtc, interval, cycles, osc, read, write, test
- Correct scratchpad protocol (no SKIP_ROM)
