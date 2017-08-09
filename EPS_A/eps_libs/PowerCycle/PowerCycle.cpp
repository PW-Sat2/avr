#include <hal/hal>

#include "IOMap.h"

#include "Include/PowerCycle/PowerCycle.h"

using namespace std::chrono_literals;
using namespace eps_a;
using namespace eps_a::iomap;

void eps_a::full_power_cycle() {
    LOG_WARNING("Performing full power cycle...");

    // Performing power cycle on supply lines
    PowerCycle::set();
    _delay_ms(500);
    watchdog::Watchdog::kick();
    _delay_ms(500);
    PowerCycle::reset();

    // Performing power cycle on Controller A supply lines with a hardware
    // watchdog's lower boundary violation.
    watchdog::Watchdog::kick();
    _delay_ms(30);
    watchdog::Watchdog::kick();
    _delay_ms(5);
    watchdog::Watchdog::kick();

    LOG_FATAL("Full power cycle on lower boundary failed");

    // If the previous method failed then performing power cycle on
    // Controller A supply lines with a hardware watchdog timer overflow.
    hal::sleep_for(5s);

    LOG_FATAL(
        "Full power cycle failed (hardware watchdog failed or bypassed?).");

    LOG_FATAL("Enabling internal watchdog");

    PowerCycle::set();
    hal::Watchdog::enable(hal::Watchdog::Period::p15ms);

    // TODO(pkuligowski): what now?
}
