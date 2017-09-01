#include "hardware/mock.h"
#include "logger.h"

namespace pld {
namespace hardware {

using namespace mock;
using namespace std::chrono_literals;

std::array<std::uint16_t, 20> mock::adc_channels;
using obc_int_pin = hal::DigitalIO::GPIO<33>;
using led1        = hal::DigitalIO::GPIO<36>;

void Mock::init() {
    obc_int_pin::init(hal::DigitalIO::Mode::OUTPUT);
    led1::init(hal::DigitalIO::Mode::OUTPUT);
    this->obc_interrupt_reset();
}

void Mock::read_adc(std::initializer_list<ChannelDescriptor> channels) {
    for (auto ch : channels) {
        uint8_t channel_ = static_cast<uint8_t>(ch.channel);

        *ch.data = adc_channels[channel_];

        hal::sleep_for(10ms);
    }
}

void Mock::radfet_on() {
    hal::sleep_for(100ms);
}

Interface::RadfetMeasurement Mock::radfet_read() {
    RadfetMeasurement rf;
    rf.measurement.temperature = 0xBAAAAD;
    rf.measurement.vth         = {0xDEAD78, 0xBEEFED, 0x1CF00D};

    for (int i = 0; i < 4 * 100; ++i) {
        hal::sleep_for(60ms);
        external_watchdog_kick();
    }

    return rf;
}

void Mock::radfet_off() {
    hal::sleep_for(100ms);
}


void Mock::external_watchdog_kick() {
    LOG_INFO("WDT");
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
}  // namespace pld
