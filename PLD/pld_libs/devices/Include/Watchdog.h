#ifndef PLD_PLD_LIBS_DEVICES_INCLUDE_WATCHDOG_H_
#define PLD_PLD_LIBS_DEVICES_INCLUDE_WATCHDOG_H_

#include <hal/hal>

namespace pld {
namespace devices {

/*!
 * TPS3813 Watchdog handling module.
 * This module used Timer1 to implement periodic tick to TPS3813.
 * User should invoke kick() method as fast as possible.
 * @tparam tps TPS3813 watchdog type.
 */
template<typename tps>
struct Watchdog {
    /*!
     * Sets up Timer1 used by this module.
     * Should be invoked once at the start of the program.
     */
    static void init() {
        tps::init();
        hal::mcu::Timer1::init(hal::mcu::Timer1::Prescaler::DIV_64,
                               hal::mcu::Timer1::Mode::Normal);
        TIMSK1 = 0;  // turn off all interrupts on Timer1
    }

    /*!
     * This method kicks watchdog, when it is needed.
     * User should invoke it as frequent as possible.
     */
    static void kick() {
        if (TIFR1 & (1 << TOV1)) {  // Timer1 overflow
            TCNT1 = 0;              // reset Timer1 counter
            TIFR1 = (1 << TOV1);    // clear Timer1 overflow flag
            tps::kick();
        }
    }
};

}  // namespace devices
}  // namespace pld

#endif  // PLD_PLD_LIBS_DEVICES_INCLUDE_WATCHDOG_H_
