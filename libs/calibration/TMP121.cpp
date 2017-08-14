#include "calibration/TMP121.h"


float avr::calibration::tmp121::raw_to_celsius(uint13_t raw) {
    uint16_t sensor_data = raw;
    uint16_t mask        = hal::libs::power_of_two<12>();
    int16_t data         = -(sensor_data & mask) + (sensor_data & ~mask);

    return static_cast<float>(data) * 0.0625;
}
