#include <algorithm>

#include "IOMap.h"

#include "eps.h"
#include "settings.h"

#include "calibration/Adc.h"
#include "calibration/LMT87.h"
#include "calibration/TMP121.h"
#include "calibration/VoltageDivider.h"

using namespace avr;

void ControllerSpecialisation::init() {
    eps::IOMap::Mux::init();
}

void ControllerSpecialisation::each_33ms() {
    Eps::TelemetryUpdater::update_mppt();
}


using InternalADC = avr::calibration::Adc<10, 3000>;

constexpr static auto tmp121 = [](auto raw) {
    return avr::calibration::tmp121::raw_to_celsius(raw);
};

constexpr static auto lmt87 = [](auto x) {
    auto voltage = InternalADC::reading_to_voltage(x);
    return avr::calibration::lmt87::volts_to_celsius(voltage);
};

float ControllerSpecialisation::max_eps_temperature() {
    auto tm = avr::Eps::telemetry.general.get();
    return std::max({lmt87(tm.dcdc_3v3_temperature),
                     lmt87(tm.dcdc_5v_temperature),
                     lmt87(tm.battery_controller.temperature),
                     tmp121(tm.battery_pack.temperature_a),
                     tmp121(tm.battery_pack.temperature_b),
                     lmt87(tm.controller_a.temperature)});
}

float ControllerSpecialisation::battery_temperature() {
    auto tm = avr::Eps::telemetry.general.get();

    return std::max({tmp121(tm.battery_pack.temperature_a),
                     tmp121(tm.battery_pack.temperature_b)});
}

float ControllerSpecialisation::battery_voltage() {
    constexpr static VoltageDivider voltage_divider(150e3, 470e3);

    auto tm = avr::Eps::telemetry.general.get();

    return voltage_divider.lower_to_full(InternalADC::reading_to_voltage(
        tm.battery_controller.controller_a_voltage));
}
