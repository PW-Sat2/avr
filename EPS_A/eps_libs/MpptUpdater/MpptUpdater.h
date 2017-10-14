#ifndef EPS_A_EPS_LIBS_MPPTUPDATER_MPPTUPDATER_H_
#define EPS_A_EPS_LIBS_MPPTUPDATER_MPPTUPDATER_H_

#include <cmath>

#include <calibration/Adc.h>
#include <hal/hal>
#include "calibration/VoltageDivider.h"
#include "logger.h"
#include "telemetry.h"

namespace eps {


constexpr static std::uint16_t voltage_to_adc_reading(float voltage) {
    constexpr VoltageDivider voltage_divider(1e3, 4.7e3);
    float voltage_on_adc_input = voltage_divider.full_to_lower(voltage);
    float adc_reading          = voltage_on_adc_input * 4095. / 3.3;

    return adc_reading;
}

namespace MpptSettings {
struct X {
    static constexpr std::int16_t dac_lower_boundary    = 750;
    static constexpr std::int16_t dac_upper_boundary    = 4080;
    static constexpr std::uint16_t dac_perturb          = 3;
    static constexpr std::uint16_t dac_sweep_down_value = 50;
    static constexpr std::uint16_t dac_sweep_up_value   = 10;
    static constexpr std::int32_t max_delta_power       = 50000;
    static constexpr std::uint16_t min_solar_voltage = voltage_to_adc_reading(3.3);
    static constexpr std::uint16_t min_output_voltage =
        voltage_to_adc_reading(6.5);
};

struct Y {
    static constexpr std::int16_t dac_lower_boundary    = 1500;
    static constexpr std::int16_t dac_upper_boundary    = 4080;
    static constexpr std::uint16_t dac_perturb          = 6;
    static constexpr std::uint16_t dac_sweep_down_value = 50;
    static constexpr std::uint16_t dac_sweep_up_value   = 10;
    static constexpr std::int32_t max_delta_power       = 100000;
    static constexpr std::uint16_t min_solar_voltage = voltage_to_adc_reading(5.3);
    static constexpr std::uint16_t min_output_voltage =
        voltage_to_adc_reading(6.7);
};
}  // namespace MpptSettings

template<typename MpptChannelSettings>
struct MpptUpdater {
    std::uint32_t last_power;
    std::int16_t dac;
    std::int8_t last_perturb;

    std::uint16_t solar_voltage, solar_current, output_voltage;

    MpptUpdater(uint12_t starting_point = 4080)
        : last_power(1),
          dac(starting_point),
          last_perturb(MpptChannelSettings::dac_perturb) {
    }


    void update_tm(const eps::Telemetry::SingleMpptChannel& tm) {
        solar_voltage  = tm.solar_voltage;
        solar_current  = tm.solar_current;
        output_voltage = tm.output_voltage;
    }

    /*!
     * This function detects current state and its responsible for switching
     * between modes. This function is called at least 20 times/s with specified
     * intervals.
     * @param single_mppt_telemetry - current MPPT telemetry for current MPPT
     * channel.
     * @return new DAC set.
     */
    uint12_t tick(const eps::Telemetry::SingleMpptChannel& single_mppt_telemetry) {
        update_tm(single_mppt_telemetry);

        if (is_solar_panel_overloaded()) {
            mpp_restart();
        } else if (is_solar_panel_underloaded()) {
            dac_sweep_down();
        } else {
            mpp_tracking();
        }

        dac_constrain();

        return dac;
    }

    /*!
     * Perturb&Observe algorithm.
     * Based on current and previous HK, it sets new DAC value.
     * @return new DAC set.
     */
    void mpp_tracking() {
        auto power               = calculate_power();
        std::int32_t delta_power = power - last_power;

        if (is_delta_power_too_high(delta_power)) {
            dac_sweep_up();
        } else if (delta_power < 0) {
            last_perturb *= -1;
        }

        dac += last_perturb;

        last_power = power;
    }

    /*!
     * Function to sweep down DAC sets with big steps.
     */
    void dac_sweep_down() {
        dac -= MpptChannelSettings::dac_sweep_down_value;
    }

    /*!
     * Function to sweep up DAC sets with big steps.
     */
    void dac_sweep_up() {
        dac += MpptChannelSettings::dac_sweep_up_value;
    }

    /*!
     * Function to restart MPPT algorithm.
     */
    void mpp_restart() {
        dac = MpptChannelSettings::dac_upper_boundary;
    }

    /*!
     * Function to calulate power.
     * @return calulated 32bit power.
     */
    uint32_t calculate_power() {
        return static_cast<std::uint32_t>(solar_voltage) *
               static_cast<std::uint32_t>(solar_current);
    }

    /*!
     * Function to contrain DAC value into acceptable range.
     */
    void dac_constrain() {
        if (dac > MpptChannelSettings::dac_upper_boundary) {
            dac = MpptChannelSettings::dac_upper_boundary;
        }
        if (dac < MpptChannelSettings::dac_lower_boundary) {
            dac = MpptChannelSettings::dac_lower_boundary;
        }
    }

    /*!
     * Function to detect that solar panel is underloaded.
     * If DCDC's output voltage is too low and solar voltage is within
     * acceptable range for DCDC, this solar panel is underloaded.
     * @return true if this solar panel is underloaded.
     */
    bool is_solar_panel_underloaded() {
        return output_voltage < MpptChannelSettings::min_output_voltage and
               solar_voltage > MpptChannelSettings::min_solar_voltage;
    }

    /*!
     * Function to detect that solar panel is overloaded.
     * If solar voltage is too low, its below DCDC's acceptable range, this
     * means that this solar panel is overloaded.
     * @param raw_solar_voltage - actual raw solar voltage.
     * @return true if this solar panel is overloaded.
     */
    bool is_solar_panel_overloaded() {
        return solar_voltage < MpptChannelSettings::min_solar_voltage or
               dac == MpptChannelSettings::dac_lower_boundary;
    }

    /*!
     * Function to detect that delta_power is too high.
     * @param delta_power - power difference between last and actual MPPT call.
     * @return true if delta_power is too high.
     */
    bool is_delta_power_too_high(std::int32_t delta_power) {
        return labs(delta_power) > MpptChannelSettings::max_delta_power;
    }
};

}  // namespace eps

#endif  // EPS_A_EPS_LIBS_MPPTUPDATER_MPPTUPDATER_H_
