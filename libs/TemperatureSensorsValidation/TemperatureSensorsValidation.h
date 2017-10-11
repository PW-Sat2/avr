#ifndef LIBS_TEMPERATURESENSORSVALIDATION_TEMPERATURESENSORSVALIDATION_H_
#define LIBS_TEMPERATURESENSORSVALIDATION_TEMPERATURESENSORSVALIDATION_H_

#include <experimental/optional>

float lmt87(uint16_t raw);
float lmt87_adc124(uint16_t raw);
float tmp121(uint16_t raw);
float pt1000(uint16_t raw);


class TemperatureConversion {
 public:
    constexpr TemperatureConversion(float (*transfer)(uint16_t),
                                    float lower,
                                    float higher,
                                    const char* name,
                                    const uint16_t& value_to_spy)
        : lower(lower),
          higher(higher),
          name(name),
          value(value_to_spy),
          transfer(transfer) {
    }

    std::experimental::optional<float> get() const;
    std::experimental::optional<float> operator()() const;

 private:
    const float lower;
    const float higher;
    const char* name;
    const uint16_t& value;
    float (*transfer)(uint16_t);
};


float merge_temperatures(
    const std::initializer_list<std::experimental::optional<float>> temperatures);


#endif  // LIBS_TEMPERATURESENSORSVALIDATION_TEMPERATURESENSORSVALIDATION_H_
