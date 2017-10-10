#include <algorithm>

#include "IOMap.h"

#include "eps.h"
#include "settings.h"

#include "calibration.h"
#include "calibration/VoltageDivider.h"

using namespace avr;

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
    return std::max(
        {lmt87<'3', 'V', '3'>(tm.dcdc_3v3_temperature),
         lmt87<'5', 'V'>(tm.dcdc_5v_temperature),
         lmt87<'B', 'A', 'T', 'C'>(tm.battery_controller.temperature),
         tmp121<'B', 'P', '_', 'A'>(tm.battery_pack.temperature_a),
         tmp121<'B', 'P', '_', 'B'>(tm.battery_pack.temperature_b),
         lmt87<'C', 'T', 'L', '_', 'A'>(tm.controller_a.temperature),
         lmt87<'S', 'U', 'P', 'P'>(tm.controller_a.supply_temperature)});
}

float ControllerSpecialisation::battery_temperature() {
    auto tm = avr::Eps::telemetry.general.get();

    return std::max({tmp121<'B', 'P', '_', 'A'>(tm.battery_pack.temperature_a),
                     tmp121<'B', 'P', '_', 'B'>(tm.battery_pack.temperature_b)});
}

float ControllerSpecialisation::battery_voltage() {
    constexpr static VoltageDivider voltage_divider(150e3, 470e3);

    auto tm = avr::Eps::telemetry.general.get();

    return voltage_divider.lower_to_full(InternalADC::reading_to_voltage(
        tm.battery_controller.controller_a_voltage));
}

void ControllerSpecialisation::each_33ms() {
    Eps::TelemetryUpdater::update_mppt();
}

void ControllerSpecialisation::init() {
    eps::IOMap::Mux::init();

    set_sleep_mode(SLEEP_MODE_IDLE);
}
