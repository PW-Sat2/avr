#ifndef LIBS_CALIBRATION_INCLUDE_CALIBRATION_TMP121_H_
#define LIBS_CALIBRATION_INCLUDE_CALIBRATION_TMP121_H_

#include <hal/hal>

namespace avr {
namespace calibration {
namespace tmp121 {

float raw_to_celsius(const uint13_t);

}  // namespace tmp121
}  // namespace calibration
}  // namespace avr

#endif  // LIBS_CALIBRATION_INCLUDE_CALIBRATION_TMP121_H_
