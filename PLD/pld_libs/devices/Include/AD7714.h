#ifndef PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_H_
#define PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_H_

#include <hal/hal>

namespace pld {
namespace devices {
namespace AD7714 {

/*!
 * AD7714 channels.
 * Notation: AIN_positive_negative.
 * TEST channel is shorted in the device (noise measurements).
 */
enum class Channels : std::uint8_t {
    AIN_1_6 = 0,
    AIN_2_6 = 1,
    AIN_3_6 = 2,
    AIN_4_6 = 3,

    AIN_1_2 = 4,
    AIN_3_4 = 5,
    AIN_5_6 = 6,
    TEST    = 7,
};

/*!
 * AD7714 Gain options.
 * Input differential voltage is multiplied by number and measured by ADC.
 */
enum class Gain : std::uint8_t {
    GAIN_1   = 0 << 2,
    GAIN_2   = 1 << 2,
    GAIN_4   = 2 << 2,
    GAIN_8   = 3 << 2,
    GAIN_16  = 4 << 2,
    GAIN_32  = 5 << 2,
    GAIN_64  = 6 << 2,
    GAIN_128 = 7 << 2,
};

/*!
 * AD7714 device driver. Uses SPI and GPIO pin.
 * SPI settings depends on configuration pin on the device.
 *
 * Example usage:
 * \code
 * using ad7714 = AD7714::AD7714<...>;
 * ad7714::init();
 * ad7714::change_channel(AD7714::Channels::AIN_1_6, AD7714::Gain::GAIN_1);
 * while(!ad7714::data_ready());
 * auto val = ad7714::read_data_no_wait();
 * \endcode
 * @tparam SPI
 * @tparam pin_DRDY
 */
template<typename SPI, typename pin_DRDY>
class AD7714 : hal::libs::PureStatic {
 public:
    /*!
     * Initialises data ready pin as INPUT_PULLUP.
     */
    static void init() {
        pin_DRDY::init(hal::DigitalIO::Mode::INPUT_PULLUP);
    }

    /*!
     * Performs readout operation from ADC:
     *   - change channel
     *   - set up filter
     *   - perform self calibration
     *   - waits for conversion finish
     *   - reads data from DATA register
     * @param channel Channel to read from
     * @param gain Gain to be set on this channel. Default value - Gain x1.
     * @return Data read from the DATA register.
     */
    static uint24_t read_data(Channels channel, Gain gain = Gain::GAIN_1) {
        change_channel(channel, gain);

        while (!data_ready()) {
        }

        std::array<std::uint8_t, 3> data;
        read_register(Registers::DATA_REG, data);
        return (1ul * data[0] << 16ul) | (1ul * data[1] << 8) | (data[2]);
    }


 private:
    enum class Registers : std::uint8_t {
        COMM_REG             = 0 << 4,
        MODE_REG             = 1 << 4,
        FILTER_HIGH_REG      = 2 << 4,
        FILTER_LOW_REG       = 3 << 4,
        TEST_REG             = 4 << 4,
        DATA_REG             = 5 << 4,
        ZERO_SCALE_CALIB_REG = 6 << 4,
        FULL_SCALE_CALIB_REG = 7 << 4,
    };

    enum class Modes : std::uint8_t {
        NORMAL_MODE           = 0 << 5,
        SELF_CALIB            = 1 << 5,
        ZERO_SCALE_SYS_CALIB  = 2 << 5,
        FULL_SCALE_SYS_CALIB  = 3 << 5,
        SYS_OFFSET_CALIB      = 4 << 5,
        BACKGROUND_CALIB      = 5 << 5,
        ZERO_SCALE_SELF_CALIB = 6 << 5,
        FULL_SCALE_SELF_CALIB = 7 << 5,
    };

    enum class Polarity : std::uint8_t {
        BIPOLAR  = 0 << 7,
        UNIPOLAR = 1 << 7,
    };
    enum class DataLength : std::uint8_t {
        DATA_16b = 0 << 6,
        DATA_24b = 1 << 6,
    };
    enum class CurrentBoost : std::uint8_t {
        DISABLED = 0 << 5,
        ENABLED  = 1 << 5,
    };


    // Configuration
    static constexpr std::uint16_t filter = 4000;

    static constexpr Polarity polarity         = Polarity::UNIPOLAR;
    static constexpr CurrentBoost currentBoost = CurrentBoost::ENABLED;
    static constexpr DataLength data_length    = DataLength::DATA_24b;


    static Channels actual_channel;

    static_assert((filter >= 19) && (filter <= 4000),
                  "Allowed filter value is 19 - 4000");


    static void change_channel(Channels channel, Gain gain) {
        actual_channel = channel;
        set_filter();
        start_calibration(gain);
    }

    static bool data_ready() {
        return !pin_DRDY::read();
    }

    static void set_filter() {
        constexpr auto filter_high_register = num(polarity) |      //
                                              num(currentBoost) |  //
                                              num(data_length) |   //
                                              hal::libs::high_byte(filter);
        write_register(Registers::FILTER_HIGH_REG,
                       std::array<uint8_t, 1>{filter_high_register});

        constexpr auto filter_low_register = hal::libs::low_byte(filter);
        write_register(Registers::FILTER_LOW_REG, filter_low_register);
    }

    static void start_calibration(const Gain gain) {
        set_mode(Modes::SELF_CALIB, gain);
    }

    static void set_mode(const Modes mode, const Gain gain) {
        std::uint8_t mode_register_value = num(mode) | num(gain);
        write_register(Registers::MODE_REG, mode_register_value);
    }


    enum class CommRegisterOperation : std::uint8_t {
        write = (0u << 3),
        read  = (1u << 3),
    };

    static void read_register(Registers reg, gsl::span<uint8_t> data) {
        write_to_comm_register(reg, CommRegisterOperation::read);
        SPI::read(data);
    }

    static void write_register(Registers reg, gsl::span<const uint8_t> data) {
        write_to_comm_register(reg, CommRegisterOperation::write);
        SPI::write(data);
    }

    static void
    write_to_comm_register(Registers registers, CommRegisterOperation operation) {
        std::uint8_t out = num(registers) |  //
                           num(operation) |  //
                           num(actual_channel);
        SPI::write(out);
    }
};
template<typename SPI, typename pin_DRDY>
Channels AD7714<SPI, pin_DRDY>::actual_channel;

}  // namespace AD7714
}  // namespace devices
}  // namespace pld

#endif  // PLD_PLD_LIBS_DEVICES_INCLUDE_AD7714_H_
