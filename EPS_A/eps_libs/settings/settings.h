#ifndef EPS_A_EPS_LIBS_SETTINGS_SETTINGS_H_
#define EPS_A_EPS_LIBS_SETTINGS_SETTINGS_H_

#include <cstdint>
#include "MpptUpdater.h"
#include "eps.h"
#include "telemetry.h"

struct ControllerSpecialisation {
    constexpr static char side                = 'A';
    constexpr static std::uint8_t i2c_address = 0x35;
    constexpr static std::uint8_t who_am_i    = 0x61;

    static void init();
    static void each_33ms();

    static float max_eps_temperature();
    static float battery_temperature();
    static float battery_voltage();
};

#endif  // EPS_A_EPS_LIBS_SETTINGS_SETTINGS_H_
