#ifndef LIBS_POWERCYCLE_POWERCYCLE_H_
#define LIBS_POWERCYCLE_POWERCYCLE_H_

namespace eps {

template<typename PowerCyclePin, typename Watchdog>
struct FullPowerCycle {
    static void perform() {
        LOG_WARNING("Performing full power cycle...");

        // Performing power cycle on supply lines
        PowerCyclePin::set();
        _delay_ms(500);
        Watchdog::kick();
        _delay_ms(500);
        PowerCyclePin::reset();

        // Performing power cycle on Controller A supply lines with a hardware
        // watchdog's lower boundary violation.
        Watchdog::kick();
        _delay_ms(30);
        Watchdog::kick();
        _delay_ms(5);
        Watchdog::kick();

        LOG_FATAL("Full power cycle on lower boundary failed");

        // If the previous method failed then performing power cycle on
        // Controller A supply lines with a hardware watchdog timer overflow.
        hal::sleep_for(std::chrono::seconds{5});

        LOG_FATAL(
            "Full power cycle failed (hardware watchdog failed or bypassed?).");

        LOG_FATAL("Enabling internal watchdog");

        PowerCyclePin::set();
        hal::Watchdog::enable(hal::Watchdog::Period::p15ms);

        // TODO(pkuligowski): what now?
    }
};

}  // namespace eps

#endif  // LIBS_POWERCYCLE_POWERCYCLE_H_
