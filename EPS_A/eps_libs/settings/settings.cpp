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

void ControllerSpecialisation::init() {
    eps::IOMap::Mux::init();

    set_sleep_mode(SLEEP_MODE_IDLE);
}


static constexpr uint8_t number_of_mppt_phases = 3;
static uint8_t mppt_phase = number_of_mppt_phases - 1;

eps::MpptUpdater<eps::MpptSettings::Channel::X> ControllerSpecialisation::MpptUpdaters::X;
eps::MpptUpdater<eps::MpptSettings::Channel::Y> ControllerSpecialisation::MpptUpdaters::Yp;
eps::MpptUpdater<eps::MpptSettings::Channel::Y> ControllerSpecialisation::MpptUpdaters::Yn;

void ControllerSpecialisation::each_33ms() {
    Eps::TelemetryUpdater::update_mppt();

    auto tm_mppt = avr::Eps::telemetry.mppt.get();

    if (mppt_phase == 0) {
        eps::IOMap::Mppt::MpptYp::DacSpi::init();
        uint12_t mppt_new_dac_value = ControllerSpecialisation::MpptUpdaters::Yp.tick(tm_mppt.mpptyp);
        eps::IOMap::Mppt::MpptYp::Dac121::write_to_output(mppt_new_dac_value);
    } else if (mppt_phase == 1) {
        eps::IOMap::Mppt::MpptYn::DacSpi::init();
        uint12_t mppt_new_dac_value = ControllerSpecialisation::MpptUpdaters::Yn.tick(tm_mppt.mpptyn);
        eps::IOMap::Mppt::MpptYn::Dac121::write_to_output(mppt_new_dac_value);
    } else if (mppt_phase == 2) {
        eps::IOMap::Mppt::MpptX::DacSpi::init();
        uint12_t mppt_new_dac_value = ControllerSpecialisation::MpptUpdaters::X.tick(tm_mppt.mpptx);
        eps::IOMap::Mppt::MpptX::Dac121::write_to_output(mppt_new_dac_value);
    }

    if (mppt_phase == 0) {
        mppt_phase = number_of_mppt_phases;
    }
    mppt_phase--;
}