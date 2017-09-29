#ifndef LIBS_OVERHEATPROTECTION_OVERHEATPROTECTION_H_
#define LIBS_OVERHEATPROTECTION_OVERHEATPROTECTION_H_

#include "logger.h"

namespace eps {

template<typename LclCommander, std::int8_t temp_threshold>
struct OverheatProtection {
    static volatile bool enabled;

    static void disable() {
        enabled = false;
        LOG_INFO("Disable Overheat protection");
    }

    static void tick(float max_temperature) {
        if (enabled && max_temperature > temp_threshold) {
            LOG_ERROR("Overheat (%f) - disable all LCLs", max_temperature);
            LclCommander::off_all();
        }
    }
};

template<typename LclCommander, std::int8_t temp_threshold>
volatile bool OverheatProtection<LclCommander, temp_threshold>::enabled = true;

}  // namespace eps

#endif  // LIBS_OVERHEATPROTECTION_OVERHEATPROTECTION_H_
