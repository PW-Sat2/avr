#include "calibration/LMT87.h"

float avr::calibration::lmt87::volts_to_celsius(const float voltage) {
    constexpr static float offset = 194.247;
    constexpr static float slope  = -1000.0 / 13.582;

    return offset + slope * voltage;
}
