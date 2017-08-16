#ifndef LIBS_CALIBRATION_INCLUDE_CALIBRATION_ADC_H_
#define LIBS_CALIBRATION_INCLUDE_CALIBRATION_ADC_H_

#include <hal/hal>

namespace avr {
namespace calibration {

template<std::uint8_t resolution, std::uint16_t reference_in_mv>
class Adc {
 public:
    template<typename T>
    static float reading_to_voltage(const T value) {
        static_assert(std::is_convertible<T, float>::value,
                      "Type has to be convertible to float!");

        return static_cast<float>(value) * scale;
    }

 private:
    constexpr static float scale =
        static_cast<float>(reference_in_mv) / 1000.0 /
        (static_cast<float>(hal::libs::power_of_two<resolution>()) - 1);
};

}  // namespace calibration
}  // namespace avr

#endif  // LIBS_CALIBRATION_INCLUDE_CALIBRATION_ADC_H_
