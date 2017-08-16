#ifndef LIBS_CALIBRATION_INCLUDE_CALIBRATION_LMT87_H_
#define LIBS_CALIBRATION_INCLUDE_CALIBRATION_LMT87_H_

#include <hal/hal>

namespace avr {
namespace calibration {
namespace lmt87 {

float volts_to_celsius(const float);

}  // namespace lmt87
}  // namespace calibration
}  // namespace avr

#endif  // LIBS_CALIBRATION_INCLUDE_CALIBRATION_LMT87_H_
