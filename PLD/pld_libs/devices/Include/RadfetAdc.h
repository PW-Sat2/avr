#ifndef PLD_PLD_LIBS_DEVICES_INCLUDE_RADFETADC_H_
#define PLD_PLD_LIBS_DEVICES_INCLUDE_RADFETADC_H_

#include <hardware/interface.h>
#include <hal/hal>
#include "AD7714.h"

namespace pld {
namespace devices {
namespace AD7714 {

/*!
 * Specialised driver for AD7714 for PLD board.
 * Implements watchdog kicking and timeout functionality.
 * @tparam Adc AD7714 class to use
 * @tparam Watchdog Watchdog to kick.
 */
template<typename Adc, typename Watchdog>
struct RadfetAdc {
    /*!
     * Timeout of ADC readout operation (waiting for DATA_READY line)
     */
    constexpr static std::chrono::milliseconds timeout{8000};
    /*!
     * Time diffence between two successive checks of DATA_READY line
     */
    constexpr static std::chrono::milliseconds granulation{1};

    /*!
     * Initialises ADC and Watchdog.
     */
    static void init() {
        Adc::init();
        Watchdog::init();
    }

    /*!
     * Result structure of read operation
     */
    struct Result {
        /*!
         * Default ctor - no timeout, data uninitialised
         */
        Result() : timeout{0} {
        }

        /*!
         * True, if timeout occurred.
         */
        bool timeout;

        /*!
         * Value read from ADC.
         * Valid even if timeout occurred.
         */
        uint24_t value;
    };

    /*!
     * Read specific channel from ADC. Gain can also be set.
     * Data will be read from ADC after timeout, even if DATA_READY line is not
     * asserted.
     *
     * @param channel Channel to read from
     * @param gain Gain to be set. Default is Gain x1.
     * @return Result structure, containing timeout and data.
     */
    static Result read(Channels channel, Gain gain = Gain::GAIN_1) {
        Result result;
        Adc::setup_read(channel, gain);

        /*!
         * Mandatory delay before DRDY pin polling. See AD7714’s datasheet
         * REV. C, page 23, "Self-Calibration" sub-chapter.
         */
        hal::sleep_for(10ms);

        std::chrono::milliseconds elapsed{0};
        while (!Adc::data_ready()) {
            hal::sleep_for(granulation);
            elapsed += granulation;
            Watchdog::kick();
            if (elapsed >= timeout) {
                result.timeout = true;
                break;
            }
        }
        result.value = Adc::read_data_no_wait();
        return result;
    }
};

}  // namespace AD7714
}  // namespace devices
}  // namespace pld

#endif  // PLD_PLD_LIBS_DEVICES_INCLUDE_RADFETADC_H_
