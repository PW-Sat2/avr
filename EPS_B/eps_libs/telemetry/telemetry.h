#ifndef EPS_B_EPS_LIBS_TELEMETRY_TELEMETRY_H_
#define EPS_B_EPS_LIBS_TELEMETRY_TELEMETRY_H_

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "atomic.h"

namespace eps {

struct Telemetry {
    struct BatteryPack {
        std::uint16_t temperature;
    };

    struct BatteryController {
        std::uint16_t voltage;
    };

    struct ControllerA {
        std::uint16_t supply_voltage;
    };

    struct ControllerB {
        std::uint8_t safety_counter;
        std::uint16_t power_cycles;
        std::uint32_t uptime;
        std::uint16_t temperature;
        std::uint16_t supply_temperature;
    };

    struct General {
        BatteryPack battery_pack;
        BatteryController battery_controller;
        ControllerA controller_a;
        ControllerB controller_b;
    };

    std::uint8_t who_am_i;
    avr::Atomic<General> general;

    void init() {
        std::memset(this, 0xFF, sizeof(*this));
    }
};
static_assert(sizeof(Telemetry) == 18,
              "Incorrect size of Telemetry structure (padding?)");
static_assert(std::is_pod<Telemetry>::value, "POD");


}  // namespace eps

#endif  // EPS_B_EPS_LIBS_TELEMETRY_TELEMETRY_H_
