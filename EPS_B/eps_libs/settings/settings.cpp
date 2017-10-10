#include <algorithm>

#include "IOMap.h"

#include "eps.h"
#include "settings.h"

#include "calibration.h"

#include "calibration/VoltageDivider.h"
#include "hal/libs/device_supports/RTD.h"

using namespace avr;

template<char... name>
constexpr static float pt1000(uint16_t reading) {
    hal::libs::RTD rtd{1000};

    auto voltage    = InternalADC::reading_to_voltage(reading);
    auto resistance = 3300. * voltage / (3.0f - voltage);
    return clamp_out_of_range<name...>(rtd.temperature(resistance));
}

float ControllerSpecialisation::max_eps_temperature() {
    auto tm = avr::Eps::telemetry.general.get();
    return std::max(
        {lmt87<'C', 'T', 'R', 'L', '_', 'B'>(tm.controller_b.temperature),
         lmt87<'S', 'U', 'P', 'P'>(tm.controller_b.supply_temperature),
         pt1000<'B', 'P'>(tm.battery_pack.temperature)});
}

float ControllerSpecialisation::battery_temperature() {
    auto tm = avr::Eps::telemetry.general.get();

    return pt1000<'B', 'P'>(tm.battery_pack.temperature);
}

float ControllerSpecialisation::battery_voltage() {
    constexpr static VoltageDivider voltage_divider(150e3, 470e3);

    auto tm = avr::Eps::telemetry.general.get();

    return voltage_divider.lower_to_full(
        InternalADC::reading_to_voltage(tm.battery_controller.voltage));
}

void ControllerSpecialisation::init() {
    eps::IOMap::MpptXDisable::init(hal::DigitalIO::Mode::OUTPUT);
    eps::IOMap::MpptYpDisable::init(hal::DigitalIO::Mode::OUTPUT);
    eps::IOMap::MpptYnDisable::init(hal::DigitalIO::Mode::OUTPUT);

    eps::IOMap::MpptXDisable::set();
    eps::IOMap::MpptYpDisable::set();
    eps::IOMap::MpptYnDisable::set();

    set_sleep_mode(SLEEP_MODE_IDLE);

    // power reduction register
    PRR = (1 << PRTIM0) |    // disable Timer0
          (1 << PRUSART1) |  // disable USART1
          (1 << PRTIM1) |    // disable Timer1
          (1 << PRSPI);      // disable SPI
}
