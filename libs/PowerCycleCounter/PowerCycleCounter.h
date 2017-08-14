#ifndef LIBS_POWERCYCLECOUNTER_POWERCYCLECOUNTER_H_
#define LIBS_POWERCYCLECOUNTER_POWERCYCLECOUNTER_H_

#include "EepromCounter.h"

namespace avr {
namespace power_cycle_counters {

/*!
 * Boot-up procedure.
 * Prints log, increments power cycle counters and checks for safety counter
 * expiration. Invokes emergency sail opening procedure if safety counter
 * expired.
 * @param emergency_open_sail Pointer to function performing emergency sail
 * opening.
 */
void bootup(void (*emergency_open_sail)());

/*!
 * Structure with read counters.
 */
struct Counter {
    /*!
     * All reboot counter - since software flash.
     */
    uint16_t all;

    /*!
     * Safety counter - counts from previous reset.
     */
    uint16_t safety;
};

/*!
 * Reads data from EEPROM.
 * @return Counter values read from EEPROM.
 */
Counter get();

/*!
 * Reset safety counter to 0.
 */
void reset();

}  // namespace power_cycle_counters
}  // namespace avr

#endif  // LIBS_POWERCYCLECOUNTER_POWERCYCLECOUNTER_H_
