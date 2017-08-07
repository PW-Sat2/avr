#ifndef EPS_A_EPS_LIBS_OBCWATCHDOG_OBCWATCHDOG_H_
#define EPS_A_EPS_LIBS_OBCWATCHDOG_OBCWATCHDOG_H_

#include "SoftwareTimer.h"
#include "logger.h"

namespace eps_a {

template<void (*full_power_cycle)()>
struct ObcWatchdog {
    struct Callbacks {
        static void countdown(std::uint16_t count) {
            if (count == 30) {
                LOG_WARNING("OBC Watchdog - 30 seconds left");
            }
        }
        static void expired() {
            LOG_FATAL("OBC Watchdog expired");
            full_power_cycle();
        }
    };

    using timer = avr::SoftwareTimer<Callbacks, 5 * 60>;

    static void tick() {
        timer::tick();
    }

    static void kick() {
        LOG_INFO("Reset OBC Watchdog");
        timer::reset();
    }
};

}  // namespace eps_a
#endif  // EPS_A_EPS_LIBS_OBCWATCHDOG_OBCWATCHDOG_H_
