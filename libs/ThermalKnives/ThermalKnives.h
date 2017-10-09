#ifndef LIBS_THERMALKNIVES_THERMALKNIVES_H_
#define LIBS_THERMALKNIVES_THERMALKNIVES_H_

#include <logger.h>
#include <hal/hal>
#include "SoftwareTimer.h"

namespace eps {

/*!
 * ThermalKnives handling for EPS.
 * It is responsible for auto-turnoff Burn switches after timeout.
 * Pins used by this module are considered active-high.
 * @tparam SailPin pin connected to sail BURN switch
 * @tparam SadsPin pin connected to sads BURN switch
 */
template<typename SailPin, typename SadsPin>
class ThermalKnives : hal::libs::PureStatic {
 public:
    /*!
     * Possible BURN commands from OBC
     */
    enum class Types : std::uint8_t {
        Sail = 1,
        Sads = 2,
    };

    /*!
     * Enable specific burn switch for 120 seconds.
     * Executing this commands when burn is in progress will reset timer.
     * @param burn_id Channel to enable.
     */
    static void burn(uint8_t burn_id);

    /*!
     * Enable specific burn switch for 120 seconds.
     * Executing this commands when burn is in progress will reset timer.
     * @param burn_id Channel to enable.
     */
    static void burn(Types burn_id);

    /*!
     * Constant-time tick. Should be invoked in main loop each 1 seconds.
     */
    static void tick();

 private:
    /*!
     * Auto-off timeout in ticks.
     */
    static constexpr uint16_t AutoDisableTicks = 2 * 60;

    template<typename ThermalKnifePin, char... name>
    struct ThermalKniveTimerCallbacks {
        static void countdown(uint16_t count) {
            ThermalKnifePin::set();
            if (count == 0) {
                constexpr char tkname[] = {name..., '\0'};
                LOG_INFO("Auto disable burn switch: %s", tkname);
            }
        }

        static void expired() {
            ThermalKnifePin::reset();
        }
    };

    using sail_timer =
        avr::SoftwareTimer<ThermalKniveTimerCallbacks<SailPin, 'S', 'A', 'I', 'L'>,
                           AutoDisableTicks>;
    using sads_timer =
        avr::SoftwareTimer<ThermalKniveTimerCallbacks<SadsPin, 'S', 'A', 'D', 'S'>,
                           AutoDisableTicks>;
};


template<typename SailPin, typename SadsPin>
void ThermalKnives<SailPin, SadsPin>::burn(uint8_t burn_id) {
    switch (burn_id) {
        case num(Types::Sail):
            LOG_INFO("Enable burn switch: SAIL");
            sail_timer::reset();
            break;

        case num(Types::Sads):
            LOG_INFO("Enable burn switch: SADS");
            sads_timer::reset();
            break;
        default:
            LOG_ERROR("Unknown BURN id %d\n", burn_id);
    }
}

template<typename SailPin, typename SadsPin>
void ThermalKnives<SailPin, SadsPin>::burn(Types burn_id) {
    burn(num(burn_id));
}

template<typename SailPin, typename SadsPin>
void ThermalKnives<SailPin, SadsPin>::tick() {
    sail_timer::tick();
    sads_timer::tick();
}

}  // namespace eps

#endif  // LIBS_THERMALKNIVES_THERMALKNIVES_H_
