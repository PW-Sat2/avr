#ifndef LIBS_SOFTWARETIMER_SOFTWARETIMER_H_
#define LIBS_SOFTWARETIMER_SOFTWARETIMER_H_

#include <cstdint>
#include <hal/hal>

namespace avr {

/*!
 * Software timer count-down module.
 * Allows user, for example, to create a timeout operation, such as simple
 * watchdog module.
 *
 * Timer will always count from period_ticks value down to 0.
 *
 * User provide callbacks to module using Callback template parameter.
 * Callback should have two static methods:
 * \code
 * struct Callbacks {
 *   static void countdown(uint16_t count);
 *   static void expired();
 * };
 * \endcode
 *
 * Take note:
 *   - countdown method will be invoked each time tick() is called and timer
 * counter > 0, with decreasing parameter (from TOP-1 to 0).
 *   - expired will be invoked each time tick() is called and timer counter ==
 * 0.
 *
 * Immediately after calling countdown(0) method expired() will be called.
 * @tparam Callbacks Callbacks provided by user.
 * @tparam period_ticks TOP value of counter.
 */
template<typename Callbacks, std::uint64_t period_ticks>
class SoftwareTimer : hal::libs::PureStatic {
 public:
    /*!
     * Reset timer counter to TOP value (period_ticks).
     */
    static void reset() {
        ticks = max_value;
    }

    /*!
     * Decrement counter by 1 and invoke proper callbacks.
     */
    static void tick() {
        if (ticks > 0) {
            ticks--;
            Callbacks::countdown(ticks);
        }

        if (ticks == 0) {
            Callbacks::expired();
        }
    }

 private:
    using Type = hal::libs::type_for_value<period_ticks>;

    constexpr static Type max_value = period_ticks;
    static Type ticks;
};

template<typename Callbacks, std::uint64_t period_ticks>
typename SoftwareTimer<Callbacks, period_ticks>::Type
    SoftwareTimer<Callbacks, period_ticks>::ticks = 0;

};  // namespace avr

#endif  // LIBS_SOFTWARETIMER_SOFTWARETIMER_H_
