#ifndef LIBS_OVERHEATPROTECTION_OVERHEATPROTECTION_H_
#define LIBS_OVERHEATPROTECTION_OVERHEATPROTECTION_H_

#include "logger.h"

namespace avr {

template<typename LclCommander, std::int8_t temp_threshold>
struct OverheatProtection {
    static volatile bool enabled;

    static void disable() {
        enabled = false;
        LOG_INFO("Disable Overheat protection");
    }

    static void tick(float max_temperature) {
        if (enabled && max_temperature > temp_threshold) {
            LOG_ERROR("Overheat - disable all LCLs");
            LclCommander::off_all();
        }
    }
};

template<typename LclCommander, std::int8_t temp_threshold>
volatile bool OverheatProtection<LclCommander, temp_threshold>::enabled = true;

}  // namespace avr

#endif  // LIBS_OVERHEATPROTECTION_OVERHEATPROTECTION_H_
