#ifndef PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_DRIVER_H_
#define PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_DRIVER_H_

#include <hal/hal>
#include "AD7714.h"

namespace pld {
namespace devices {
namespace AD7714 {

template<typename adc>
struct AD7714_driver {
    static void init() {
        adc::init();
    }

    constexpr static std::chrono::milliseconds timeout{8000};
    constexpr static std::chrono::milliseconds granulation{1};

    static uint24_t read(Channels channel) {
        adc::setup_read(channel);

        std::chrono::milliseconds elapsed{0};
        while (!adc::data_ready()) {
            hal::sleep_for(granulation);
            elapsed += granulation;
            if (elapsed >= timeout) {
                return 0;
            }
        }
        return adc::read_data_no_wait();
    }
};

}  // namespace AD7714
}  // namespace devices
}  // namespace pld

#endif  // PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_DRIVER_H_
