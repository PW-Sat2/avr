#ifndef PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_DRIVER_H_
#define PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_DRIVER_H_

#include <hal/hal>
#include <hardware/interface.h>
#include "AD7714.h"

namespace pld {
namespace devices {
namespace AD7714 {

template<typename Adc, typename Watchdog>
struct AD7714_driver {
    static void init() {
        Adc::init();
    }

    constexpr static std::chrono::milliseconds timeout{8000};
    constexpr static std::chrono::milliseconds granulation{1};

    static uint24_t read(Channels channel, Gain gain = Gain::GAIN_1) {
        Adc::setup_read(channel, gain);

        std::chrono::milliseconds elapsed{0};
        while (!Adc::data_ready()) {
            hal::sleep_for(granulation);
            elapsed += granulation;
            Watchdog::kick();
            if (elapsed >= timeout) {
                return 0;
            }
        }
        return Adc::read_data_no_wait();
    }
};

}  // namespace AD7714
}  // namespace devices
}  // namespace pld

#endif  // PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_DRIVER_H_
