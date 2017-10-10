#ifndef LIBS_EPS_CALIBRATION_H_
#define LIBS_EPS_CALIBRATION_H_

#include "calibration/Adc.h"
#include "calibration/LMT87.h"
#include "calibration/TMP121.h"

using InternalADC = avr::calibration::Adc<10, 3000>;

static constexpr float failsafe_temperature = 10;

template<char... name>
static void print_temp_failed(float value) {
    char name_[] = {name..., '\0'};
    LOG_ERROR("Temperature sensor [%s] failed (%.2f)", name_, value);
}

template<char... name>
float clamp_out_of_range(float value) {
    if (value < -55. || value > 110.) {
        print_temp_failed<name...>(value);
        return failsafe_temperature;
    }
    return value;
}

template<char... name>
constexpr static float tmp121(uint13_t raw) {
    return clamp_out_of_range<name...>(
        avr::calibration::tmp121::raw_to_celsius(raw));
}

template<char... name>
constexpr static float lmt87(uint16_t x) {
    auto voltage = InternalADC::reading_to_voltage(x);
    return clamp_out_of_range<name...>(
        avr::calibration::lmt87::volts_to_celsius(voltage));
}

#endif  // LIBS_EPS_CALIBRATION_H_
