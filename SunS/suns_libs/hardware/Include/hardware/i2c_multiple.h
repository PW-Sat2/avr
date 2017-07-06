#ifndef SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_I2C_MULTIPLE_H_
#define SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_I2C_MULTIPLE_H_

#include <hal/hal>

namespace SoftI2CMulti {
enum class Action : std::uint8_t {
    START_WRITE = 0,
    START_READ = 1
};

template<typename _scl, typename _sda_a, typename _sda_b, typename _sda_c, typename _sda_d, uint32_t _timeout>
class SoftI2CMulti : hal::libs::PureStatic {
 public:
    static void init() {
        _scl::init(hal::DigitalIO::Mode::INPUT);
        _sda_a::init(hal::DigitalIO::Mode::INPUT);
        _sda_b::init(hal::DigitalIO::Mode::INPUT);
        _sda_c::init(hal::DigitalIO::Mode::INPUT);
        _sda_d::init(hal::DigitalIO::Mode::INPUT);
    }

    static std::uint8_t start(std::uint8_t address, const Action start_action) {
        _scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);
        sda_pull_low();
        _delay_loop_1(hDelay);

        return write((address << 1) | num(start_action));
    }

    static void stop() {
        sda_pull_low();
        _delay_loop_1(hDelay);
        _scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(qDelay);
        sda_hi_z();
        _delay_loop_1(hDelay);
    }

    static std::uint8_t write(std::uint8_t data) {
        for (int8_t i = 7; i >= 0; i--) {
            shift_bit(hal::libs::read_bit(data, i));
        }

        _scl::reset();
        _scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(qDelay);
        sda_hi_z();
        _delay_loop_1(hDelay);

        _scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);

        std::uint8_t result = read_ack();

        _scl::reset();
        _scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(hDelay);

        return result;
    }

    static bool read(std::array<std::uint8_t, 4>& data, bool ack) {
        std::array<std::uint8_t, 4> SDA_read_data = {0};
        for (int8_t i = 7; i >= 0; i--) {
            _scl_low_high();
            write_bit_runtime(SDA_read_data[0], _sda_a::read(), i);
            write_bit_runtime(SDA_read_data[1], _sda_b::read(), i);
            write_bit_runtime(SDA_read_data[2], _sda_c::read(), i);
            write_bit_runtime(SDA_read_data[3], _sda_d::read(), i);
        }

        _scl::reset();
        _scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(qDelay);

        if (true == ack) {
            sda_pull_low();
        } else {
            sda_hi_z();
        }

        _delay_loop_1(hDelay);

        _scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);

        _scl::reset();
        _scl::init(hal::DigitalIO::Mode::OUTPUT);

        sda_hi_z();
        _delay_loop_1(hDelay);
        data = SDA_read_data;

        return false;
    }

 private:
    static constexpr std::uint8_t qDelay{3};
    static constexpr std::uint8_t hDelay{5};

    static void sda_pull_low() {
        _sda_a::reset();
        _sda_b::reset();
        _sda_c::reset();
        _sda_d::reset();

        _sda_a::init(hal::DigitalIO::Mode::OUTPUT);
        _sda_b::init(hal::DigitalIO::Mode::OUTPUT);
        _sda_c::init(hal::DigitalIO::Mode::OUTPUT);
        _sda_d::init(hal::DigitalIO::Mode::OUTPUT);
    }

    static void sda_hi_z() {
        _sda_a::init(hal::DigitalIO::Mode::INPUT);
        _sda_b::init(hal::DigitalIO::Mode::INPUT);
        _sda_c::init(hal::DigitalIO::Mode::INPUT);
        _sda_d::init(hal::DigitalIO::Mode::INPUT);
    }

    template<typename T1, typename T2>
    static inline void write_bit_runtime(T1& dest, bool val, T2 pos) {
        if (val) {
            dest |= (val << pos);
        } else {
            dest &= ~(val << pos);
        }
    }

    static void shift_bit(bool data_bit) {
        _scl::reset();
        _scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(qDelay);

        if (data_bit) {
            sda_hi_z();
        } else {
            sda_pull_low();
        }

        _delay_loop_1(hDelay);
        _scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);
    }

    static std::uint8_t read_ack() {
        std::uint8_t ack = 0;
        hal::libs::write_bit<0>(ack, _sda_a::read());
        hal::libs::write_bit<1>(ack, _sda_b::read());
        hal::libs::write_bit<2>(ack, _sda_c::read());
        hal::libs::write_bit<3>(ack, _sda_d::read());
        return ack;
    }

    static void _scl_low_high() {
        _scl::reset();
        _scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(hDelay);
        _scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);
    }

};
} // namespace SoftI2CMulti

#endif  // SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_I2C_MULTIPLE_H_
