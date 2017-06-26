#include "hardware/mock.h"

namespace suns {
namespace hardware {

using namespace mock;

using obc_int_pin = hal::DigitalIO::GPIO<33>;
using led1        = hal::DigitalIO::GPIO<36>;

void Mock::init() {
    obc_int_pin::init(hal::DigitalIO::Mode::OUTPUT);
    led1::init(hal::DigitalIO::Mode::OUTPUT);
    this->obc_interrupt_reset();
}

/*Telemetry::LightData Mock::read_radfet() {
    Telemetry::LightData rf;
    for (uint8_t i = 0; i < 10; ++i) {
        _delay_ms(200);
        this->watchdog_kick();
    }
    return rf;
}*/

void Mock::watchdog_kick() {
    hal::Watchdog::kick();
}

Telemetry::ALS Mock::als_measure(uint8_t gain, uint8_t itime) {

}

void Mock::obc_interrupt_set() {
    obc_int_pin::set();
    led1::set();
}

void Mock::obc_interrupt_reset() {
    obc_int_pin::reset();
    led1::reset();
}

}  // namespace hardware
}  // namespace suns
