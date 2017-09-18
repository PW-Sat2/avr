#ifndef EPS_A_EPS_LIBS_MPPTUPDATER_MPPTUPDATER_H_
#define EPS_A_EPS_LIBS_MPPTUPDATER_MPPTUPDATER_H_

#include <hal/hal>
#include "telemetry.h"
#include "logger.h"
#include "calibration/VoltageDivider.h"

namespace eps {

class MpptSettings {
 private:
    template<uint16_t lower_boundary, uint16_t upper_boundary, uint16_t perturb,
                uint16_t sweep_down_value, uint16_t sweep_up_value, int32_t maximum_delta_power,
                uint16_t minimum_solar_voltage_mv, uint16_t minimum_output_voltage_mv>

    struct SingleChannelSettings {
        static constexpr std::uint16_t dac_lower_boundary = lower_boundary;
        static constexpr std::uint16_t dac_upper_boundary = upper_boundary;
        static constexpr std::uint16_t dac_perturb = perturb;
        static constexpr std::uint16_t dac_sweep_down_value = sweep_down_value;
        static constexpr std::uint16_t dac_sweep_up_value = sweep_up_value;
        static constexpr std::int32_t max_delta_power = maximum_delta_power;
        static constexpr float min_solar_voltage = minimum_solar_voltage_mv / 1000.0;
        static constexpr float min_output_voltage = minimum_output_voltage_mv / 1000.0;
    };

 public:
    struct Channel {
        using X = SingleChannelSettings<750, 4080, 3, 50, 10, 50000, 3300, 6500>;
        using Y = SingleChannelSettings<1500, 4080, 6, 50, 10, 100000, 5300, 6500>;
    };
};

template<typename MpptChannelSettings>
struct MpptUpdater {
    std::uint32_t last_power;
    std::uint16_t last_dac_set;
    std::int8_t last_dac_perturb;

    MpptUpdater(uint12_t starting_point = 4080):last_power(1), last_dac_set(starting_point),
                                                last_dac_perturb(MpptChannelSettings::dac_perturb) { }

    /*!
    * This function detects current state and its responsible for switching between modes.
    * This function is called at least 20 times/s with specified intervals.
    * @param single_mppt_telemetry - current MPPT telemetry for current MPPT channel.
    * @return new DAC set.
    */
    uint12_t tick(const eps::Telemetry::SingleMpptChannel& single_mppt_telemetry) {
        if (is_solar_panel_overloaded(single_mppt_telemetry.solar_voltage)) {
            return mpp_restart();
        } else if (is_solar_panel_underloaded(single_mppt_telemetry.solar_voltage,
                                            single_mppt_telemetry.output_voltage)) {
            return dac_sweep_down();
        }

        return mpp_tracking(single_mppt_telemetry);
    }

    /*!
    * Perturb&Observe algorithm.
    * Based on current and previous HK, it sets new DAC value.
    * @param single_mppt_telemetry - current MPPT telemetry for current MPPT channel.
    * @return new DAC set.
    */
    uint12_t mpp_tracking(const eps::Telemetry::SingleMpptChannel& single_mppt_telemetry) {
        std::uint32_t power = calculate_power(single_mppt_telemetry.solar_voltage, single_mppt_telemetry.solar_current);
        std::int32_t delta_power = power - last_power;

        if (is_delta_power_too_high(delta_power)) {
            dac_sweep_up();
        } else if (delta_power < 0) {
            last_dac_perturb *= -1;
        }

        last_dac_set = last_dac_set + last_dac_perturb;
        last_dac_set = dac_value_constrain(last_dac_set);

        last_power = power;
        return last_dac_set;
    }

    /*!
    * Function to sweep down DAC sets with big steps.
    * @return new DAC set.
    */
    uint12_t dac_sweep_down() {
        last_dac_set = dac_value_constrain(last_dac_set - MpptChannelSettings::dac_sweep_down_value);
        return last_dac_set;
    }

    /*!
    * Function to sweep up DAC sets with big steps.
    * @return new DAC set.
    */
    uint12_t dac_sweep_up() {
        last_dac_set = dac_value_constrain(last_dac_set + MpptChannelSettings::dac_sweep_up_value);
        return last_dac_set;
    }

    /*!
    * Function to restart MPPT algorithm.
    * @return new DAC set.
    */
    uint12_t mpp_restart() {
        last_dac_set = MpptChannelSettings::dac_upper_boundary;
        return last_dac_set;
    }

    /*!
    * Function to calulate power.
    * @param raw_solar_voltage - raw solar voltage.
    * @param raw_solar_current - raw solar current.
    * @return calulated 32bit power.
    */
    uint32_t calculate_power(std::uint16_t raw_solar_voltage, std::uint16_t raw_solar_current) {
        return (std::uint32_t)raw_solar_voltage * (std::uint32_t)raw_solar_current;
    }

    /*!
    * Function to contrain DAC value into acceptable range.
    * @param dac_value - DAC value to contrain.
    * @return constrained DAC value.
    */
    uint12_t dac_value_constrain(uint12_t dac_value) {
        if (dac_value > MpptChannelSettings::dac_upper_boundary) {
            dac_value = MpptChannelSettings::dac_upper_boundary;
        }
        if (dac_value < MpptChannelSettings::dac_lower_boundary) {
            dac_value = MpptChannelSettings::dac_lower_boundary;
        }
        return dac_value;
    }

    /*!
    * Function to detect that solar panel is underloaded.
    * If DCDC's output voltage is too low and solar voltage is within 
    * acceptable range for DCDC, this solar panel is underloaded.
    * @param raw_solar_voltage - actual raw solar voltage.
    * @param raw_output_voltage - actual raw DCDC output voltage.
    * @return true if this solar panel is underloaded.
    */
    bool is_solar_panel_underloaded(std::uint16_t raw_solar_voltage, std::uint16_t raw_output_voltage) {
        float actual_solar_voltage = mppt_solar_voltage(raw_solar_voltage);
        float actual_output_voltage = mppt_output_voltage(raw_output_voltage);

        if ((actual_output_voltage < MpptChannelSettings::min_output_voltage)
            and(actual_solar_voltage > MpptChannelSettings::min_solar_voltage)) {
            return true;
        }
        return false;
    }

    /*!
    * Function to detect that solar panel is overloaded.
    * If solar voltage is too low, its below DCDC's acceptable range, this means that 
    * this solar panel is overloaded.
    * @param raw_solar_voltage - actual raw solar voltage.
    * @return true if this solar panel is overloaded.
    */
    bool is_solar_panel_overloaded(std::uint16_t raw_solar_voltage) {
        float actual_solar_voltage = mppt_solar_voltage(raw_solar_voltage);

        if ((actual_solar_voltage < MpptChannelSettings::min_solar_voltage)
            or(last_dac_set == MpptChannelSettings::dac_lower_boundary)) {
            return true;
        }
        return false;
    }

    /*!
    * Function to detect that current_delta_power is too high.
    * @param curret_delta_power - power difference between last and actual MPPT call.
    * @return true if curret_delta_power is too high.
    */
    bool is_delta_power_too_high(std::int32_t curret_delta_power) {
        if ((curret_delta_power > MpptChannelSettings::max_delta_power)
            or(curret_delta_power < (-MpptChannelSettings::max_delta_power))) {
            return true;
        }
        return false;
    }

    /*!
    * Function to calculate any voltage in MPPT converter. Valid only for MPPT ADC.
    * @param adc_value - ADC readout.
    * @return output voltage in volts.
    */
    float mppt_voltage(std::uint16_t adc_value) {
        constexpr static VoltageDivider voltage_divider(1e3, 4.7e3);

        auto max_adc_value = 4095.0;
        auto adc_reference_voltage = 3.3;

        return voltage_divider.lower_to_full((adc_value / max_adc_value) * adc_reference_voltage);
    }

    /*!
    * Function to calculate output voltage.
    * @param raw_output_voltage - raw output voltage (ADC readout).
    * @return output voltage in volts.
    */
    float mppt_output_voltage(std::uint16_t raw_output_voltage) {
        return mppt_voltage(raw_output_voltage);
    }

    /*!
    * Function to calculate solar voltage.
    * @param raw_solar_voltage - raw solar voltage (ADC readout).
    * @return solar voltage in volts.
    */
    float mppt_solar_voltage(std::uint16_t raw_solar_voltage) {
        return mppt_voltage(raw_solar_voltage);
    }
};

}  // namespace eps

#endif  // EPS_A_EPS_LIBS_MPPTUPDATER_MPPTUPDATER_H_
