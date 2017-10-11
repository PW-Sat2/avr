#include <calibration/Adc.h>
#include <calibration/VoltageDivider.h>

#include "IOMap.h"

#include "eps.h"
#include "settings.h"

#include "Temperatures.h"

using namespace avr;
using namespace avr::temperatures;

float ControllerSpecialisation::max_eps_temperature() {
    auto temperatures = {battery_pack_a(),
                         battery_pack_b(),
                         dcdc3v3(),
                         dcdc5v(),
                         batc(),
                         ctrla(),
                         supply(),
                         mppt_x(),
                         mppt_yp(),
                         mppt_yn()};

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
    return merge_temperatures({battery_pack_a(), battery_pack_b()});
}

float ControllerSpecialisation::battery_voltage() {
    constexpr static VoltageDivider voltage_divider(150e3, 470e3);

    auto tm = avr::Eps::telemetry.general.get();

    using InternalADC = avr::calibration::Adc<10, 3000>;

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
