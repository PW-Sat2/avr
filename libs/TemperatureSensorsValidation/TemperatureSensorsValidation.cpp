#include <cmath>

#include "logger.h"

#include <hal/libs/device_supports/RTD.h>
#include "calibration/Adc.h"
#include "calibration/LMT87.h"
#include "calibration/TMP121.h"

#include "TemperatureSensorsValidation.h"

static constexpr float failsafe_temperature = 10;

using InternalADC = avr::calibration::Adc<10, 3000>;

using OptFloat = std::experimental::optional<float>;

float lmt87(uint16_t raw) {
    auto voltage = InternalADC::reading_to_voltage(raw);
    return avr::calibration::lmt87::volts_to_celsius(voltage);
}

using Adc124 = avr::calibration::Adc<12, 3300>;
float lmt87_adc124(uint16_t raw) {
    auto voltage = Adc124::reading_to_voltage(raw);
    return avr::calibration::lmt87::volts_to_celsius(voltage);
}

float tmp121(uint16_t raw) {
    return avr::calibration::tmp121::raw_to_celsius(raw);
}

float pt1000(uint16_t raw) {
    hal::libs::RTD rtd{1000};

    auto voltage    = InternalADC::reading_to_voltage(raw);
    auto resistance = 3300. * voltage / (3.0f - voltage);
    return rtd.temperature(resistance);
}


OptFloat TemperatureConversion::get() const {
    auto temperature = transfer(value);

    if (isfinite(temperature) && lower < temperature && temperature < higher) {
        return temperature;
    }

#if LOG_LEVEL >= LOG_LEVEL_ERROR
    printf_P(PSTR("[ERROR]  Temperature sensor ["));
    printf_P(name);
    printf_P(PSTR("] failed: %f!\n"), temperature);
#endif

    return {};
}

OptFloat TemperatureConversion::operator()() const {
    return this->get();
}


float merge_temperatures(const std::initializer_list<OptFloat> list) {
    bool any_valid = false;
    float temp     = -std::numeric_limits<float>::max();

    for (const auto& x : list) {
        if (x) {
            any_valid = true;
            temp      = std::max(temp, *x);
        }
    }

    if (any_valid == false) {
        LOG_FATAL("All tempetarure sensors failed!");
        return failsafe_temperature;
    }

    return temp;
}
