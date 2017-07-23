#include <hardware/real.h>
#include <hal/hal>
#include "logger.h"

using namespace hal;
using namespace devices;
using namespace libs;

using interrupt = hal::DigitalIO::GPIO<42>;


void suns::hardware::RealHardware::init() {
    interrupt::reset();
    interrupt::init(hal::DigitalIO::Mode::OUTPUT);
}

void suns::hardware::RealHardware::measure() {
}

void suns::hardware::RealHardware::watchdog_kick() {
    hal::Watchdog::kick();
}

void suns::hardware::RealHardware::obc_interrupt_set() {
    interrupt::set();
}

void suns::hardware::RealHardware::obc_interrupt_reset() {
    interrupt::reset();
}
