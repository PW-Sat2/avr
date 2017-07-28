#ifndef EPS_A_EPS_LIBS_THERMALKNIVES_THERMALKNIVES_H_
#define EPS_A_EPS_LIBS_THERMALKNIVES_THERMALKNIVES_H_

#include <logger.h>
#include <hal/hal>
#include "SoftwareTimer.h"

namespace eps_a {

template<typename ThermalKnifeInterface, char... name>
struct ThermalKniveTimerCallbacks {
    static void countdown(uint16_t count) {
        ThermalKnifeInterface::enable();
        if (count == 0) {
            const char tkname[] = {name..., '\0'};
            LOG_INFO("Auto disable burn switch: %s", tkname);
        }
    }

    static void expired() {
        ThermalKnifeInterface::disable();
    }
};


template<typename SailInterface, typename SadsInterface>
struct ThermalKnives : hal::libs::PureStatic {
    enum class Types : std::uint8_t {
        Sail = 0,
        Sads = 1,
    };

    static void burn(Types burn_id);
    static void tick();


    static constexpr uint16_t AutoDisableTicks = 2 * 60;

    using sail_timer = SoftwareTimer<
        ThermalKniveTimerCallbacks<SailInterface, 'S', 'A', 'I', 'L', 'm', 'a', 'i', 'n'>,
        AutoDisableTicks>;
    using sads_timer = SoftwareTimer<
        ThermalKniveTimerCallbacks<SadsInterface, 'S', 'A', 'D', 'S', 'm', 'a', 'i', 'n'>,
        AutoDisableTicks>;
};


template<typename SailInterface, typename SadsInterface>
void ThermalKnives<SailInterface, SadsInterface>::burn(Types burn_id) {
    switch (burn_id) {
        case Types::Sail:
            LOG_INFO("Enable burn switch: SAILmain");
            sail_timer::reset();
            break;

        case Types::Sads:
            LOG_INFO("Enable burn switch: SADSmain");
            sads_timer::reset();
            break;
    }
}

template<typename SailInterface, typename SadsInterface>
void ThermalKnives<SailInterface, SadsInterface>::tick() {
    sail_timer::tick();
    sads_timer::tick();
}

}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_THERMALKNIVES_THERMALKNIVES_H_
