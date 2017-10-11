#include <calibration/Adc.h>
#include <algorithm>

#include "IOMap.h"

#include "eps.h"
#include "settings.h"

#include "calibration/VoltageDivider.h"

#include "Temperatures.h"

using namespace avr;
using namespace avr::temperatures;

float ControllerSpecialisation::max_eps_temperature() {
    auto temperatures = {ctrlb(), supply(), battery_pack()};

    auto merged = merge_temperatures(temperatures);

#if LOG_LEVEL == LOG_LEVEL_DEBUG
    printf_P(PSTR("Tmps: "));
    for (const auto& i : temperatures) {
        printf_P(PSTR("%.2f "), *i);
    }
    printf_P(PSTR("-> %.2f\n"), merged);
#endif

    return merged;
}

float ControllerSpecialisation::battery_temperature() {
    return merge_temperatures({battery_pack()});
}

float ControllerSpecialisation::battery_voltage() {
    using InternalADC = avr::calibration::Adc<10, 3000>;
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
