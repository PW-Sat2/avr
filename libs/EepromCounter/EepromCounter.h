#ifndef LIBS_EEPROMCOUNTER_EEPROMCOUNTER_H_
#define LIBS_EEPROMCOUNTER_EEPROMCOUNTER_H_

#include <hal/hal>

#include "logger.h"
#include "redundancy.h"

namespace avr {
/*!
 * EEPROM-based counter. Holds up value between reboots.
 * This module stores counter 5 times and performs majority voting.
 * @tparam T Type of the counter. Should be unsigned integral type (such as
 * uint8_t, uint16_t etc)
 */
template<typename T>
class EepromCounter {
 public:
    static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
                  "Counter type has to be of integral unsigned type");

    /*!
     * Type of underlying EEPROM storage.
     */
    using type = std::array<T, 5>;

    /*!
     * Constructor.
     * @param data_eeprom Reference to object in EEPROM memory (declared with
     * EEMEM attribute).
     */
    EepromCounter(type& data_eeprom) : wrapper{data_eeprom} {
    }

    /*!
     * Read counter and perform majority voting on 5 elements.
     * @return Counter value after voting.
     */
    uint16_t get() {
        auto data = wrapper.read();
        auto now  = avr::redundancy::Correct(data);

        return now;
    }

    /*!
     * Increment counter in EEPROM memory.
     */
    void increment() {
        auto now = get() + 1;

        type data;
        data.fill(now);

        wrapper = data;

        if (get() != now) {
            LOG_FATAL("Cannot write to EEPROM");
        }
    }

    /*!
     * Reset counter to 0.
     */
    void reset() {
        wrapper = {0, 0, 0, 0, 0};
    }

 private:
    hal::EepromWrapper<std::array<T, 5>> wrapper;
};

}  // namespace avr

#endif  // LIBS_EEPROMCOUNTER_EEPROMCOUNTER_H_
