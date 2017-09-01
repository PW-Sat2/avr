#include <algorithm>

#include "IOMap.h"

#include "eps.h"
#include "settings.h"

#include "calibration/Adc.h"
#include "calibration/LMT87.h"
#include "calibration/TMP121.h"

#include <hal/libs/device_supports/RTD.h>

using namespace avr;

using InternalADC = avr::calibration::Adc<10, 3000>;

constexpr static auto tmp121 = [](auto raw) {
    return avr::calibration::tmp121::raw_to_celsius(raw);
};

constexpr static auto lmt87 = [](auto x) {
    auto voltage = InternalADC::reading_to_voltage(x);
    return avr::calibration::lmt87::volts_to_celsius(voltage);
};

constexpr static auto pt1000 = [](auto reading) {
    hal::libs::RTD rtd{1000};

    auto voltage    = InternalADC::reading_to_voltage(reading);
    auto resistance = 3300 * voltage / (3.0f - voltage);
    return rtd.temperature(resistance);
};

float ControllerSpecialisation::max_eps_temperature() {
    auto tm = avr::Eps::telemetry.general.get();
    return std::max({lmt87(tm.controller_b.temperature),
                     lmt87(tm.controller_b.supply_temperature),
                     pt1000(tm.battery_pack.temperature)});
}

float ControllerSpecialisation::battery_temperature() {
    auto tm = avr::Eps::telemetry.general.get();

    return pt1000(tm.battery_pack.temperature);
}

float ControllerSpecialisation::battery_voltage() {
    auto tm = avr::Eps::telemetry.general.get();

    return InternalADC::reading_to_voltage(
            tm.battery_controller.voltage);
}

void ControllerSpecialisation::init() {
    eps::IOMap::MpptXDisable::init(hal::DigitalIO::Mode::OUTPUT);
    eps::IOMap::MpptYpDisable::init(hal::DigitalIO::Mode::OUTPUT);
    eps::IOMap::MpptYnDisable::init(hal::DigitalIO::Mode::OUTPUT);

    eps::IOMap::MpptXDisable::set();
    eps::IOMap::MpptYpDisable::set();
    eps::IOMap::MpptYnDisable::set();
}

