#ifndef LIBS_CALIBRATION_INCLUDE_CALIBRATION_VOLTAGEDIVIDER_H_
#define LIBS_CALIBRATION_INCLUDE_CALIBRATION_VOLTAGEDIVIDER_H_

class VoltageDivider {
 public:
    constexpr VoltageDivider(float low, float high)
        : resistance_lower{low}, resistance_higher{high} {
    }

    float lower_to_full(float lower_voltage) const {
        return lower_voltage / scale();
    }

 private:
    const float resistance_lower, resistance_higher;

    constexpr float scale() const {
        return resistance_lower / (resistance_lower + resistance_higher);
    }
};

#endif  // LIBS_CALIBRATION_INCLUDE_CALIBRATION_VOLTAGEDIVIDER_H_
