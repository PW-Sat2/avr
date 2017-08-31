#ifndef EPS_B_EPS_LIBS_SETTINGS_SETTINGS_H_
#define EPS_B_EPS_LIBS_SETTINGS_SETTINGS_H_

#include <cstdint>

struct ControllerSpecialisation {
    constexpr static char side                = 'B';
    constexpr static std::uint8_t i2c_address = 0x36;
    constexpr static std::uint8_t who_am_i    = 0x9D;

    static void init();
    static void each_33ms() {
    }

    static float max_eps_temperature();
    static float battery_temperature();
    static float battery_voltage();
};

#endif  // EPS_B_EPS_LIBS_SETTINGS_SETTINGS_H_
