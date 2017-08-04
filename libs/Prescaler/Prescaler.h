#ifndef LIBS_PRESCALER_PRESCALER_H_
#define LIBS_PRESCALER_PRESCALER_H_

#include <cstdint>
#include <hal/hal>

namespace avr {

/*!
 * Prescaler class. Can be used to delay some operations by n-ticks.
 * It will expire once for each prescaler invocations of expired() function.
 * @tparam prescaler Prescaler to use
 */
template<uint64_t prescaler>
class Prescaler {
 public:
    /*!
     * Default ctor. Resets counter.
     */
    Prescaler() : counter{max_value} {
    }

    /*!
     * Checks if prescaler expired. If so, resets it.
     * This function will return true each prescaler times invoked
     * @return True if expired
     */
    constexpr bool expired() {
        counter--;
        if (counter == 0) {
            counter = max_value;
            return true;
        }
        return false;
    }

 private:
    static_assert(prescaler > 0, "Prescaler value has to be positive!");

    using Type = hal::libs::type_for_value<prescaler>;

    Type counter;
    constexpr static Type max_value = static_cast<Type>(prescaler);
};

};  // namespace avr

#endif  // LIBS_PRESCALER_PRESCALER_H_
