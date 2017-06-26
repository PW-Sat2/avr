#ifndef SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_I2C_MULTIPLE_H_
#define SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_I2C_MULTIPLE_H_

#include <hal/hal>

namespace SoftI2CMulti {
enum class Action : uint8_t {
    START_WRITE = 0,
    START_READ = 1
};

template<typename scl, typename sda_a, typename sda_b, typename sda_c, typename sda_d>
class SoftI2CMulti : hal::libs::PureStatic {
 public:
    static void init() {
        scl::init(hal::DigitalIO::Mode::INPUT);
        sda_a::init(hal::DigitalIO::Mode::INPUT);
        sda_b::init(hal::DigitalIO::Mode::INPUT);
        sda_c::init(hal::DigitalIO::Mode::INPUT);
        sda_d::init(hal::DigitalIO::Mode::INPUT);
    }

    static uint8_t start(uint8_t address, const Action start_action) {
        scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);
        sda_pull_low();
        _delay_loop_1(hDelay);

        return write((address << 1) | static_cast<uint8_t>(start_action));
    }

    static void stop() {
        sda_pull_low();
        _delay_loop_1(hDelay);
        scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(qDelay);
        sda_hi_z();
        _delay_loop_1(hDelay);
    }

    static void shift_bit(bool bit) {
        scl::reset();
        scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(qDelay);

        if (bit) {
            sda_hi_z();
        } else {
            sda_pull_low();
        }

        _delay_loop_1(hDelay);
        scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);
    }

    static bool wait_for_clk_stretching() {
        volatile uint16_t timeout = 0;
        while (0 == scl::read()) {
            if (timeout > 5000) {
                timeout++;
            } else {
                return true;
            }
        }
        return false;
    }

    static uint8_t read_ack() {
        uint8_t ack = 0;
        hal::libs::write_bit<0>(ack, sda_a::read());
        hal::libs::write_bit<1>(ack, sda_b::read());
        hal::libs::write_bit<2>(ack, sda_c::read());
        hal::libs::write_bit<3>(ack, sda_d::read());
        return ack;
    }

    static uint8_t write(uint8_t data) {
        uint8_t result = 0;

        for (int8_t i = 7; i >= 0; i--) {
            shift_bit(hal::libs::read_bit(data, i));

            if (wait_for_clk_stretching()) {
                hal::libs::write_bit<4>(result, 1);
            }
        }

        scl::reset();
        scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(qDelay);
        sda_hi_z();
        _delay_loop_1(hDelay);

        scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);

        result |= read_ack();

        scl::reset();
        scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(hDelay);

        return result;
    }

    static void scl_low_high() {
        scl::reset();
        scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(hDelay);
        scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);
    }

    static std::array<uint8_t, 4> read(bool ack) {
        std::array<uint8_t, 4> SDA_read_data = {0};

        for (int8_t i = 7; i >= 0; i--) {
            scl_low_high();
            while (0 == scl::read()) {
            }

            wb(SDA_read_data[0], sda_a::read(), i);
            wb(SDA_read_data[1], sda_b::read(), i);
            wb(SDA_read_data[2], sda_c::read(), i);
            wb(SDA_read_data[3], sda_d::read(), i);
        }

        scl::reset();
        scl::init(hal::DigitalIO::Mode::OUTPUT);
        _delay_loop_1(qDelay);

        if (true == ack) {
            sda_pull_low();
        } else {
            sda_hi_z();
        }

        _delay_loop_1(hDelay);

        scl::init(hal::DigitalIO::Mode::INPUT);
        _delay_loop_1(hDelay);

        scl::reset();
        scl::init(hal::DigitalIO::Mode::OUTPUT);

        sda_hi_z();
        _delay_loop_1(hDelay);

        return SDA_read_data;
    }

 private:
    static constexpr uint8_t qDelay{3};
    static constexpr uint8_t hDelay{5};

    static void sda_pull_low() {
        sda_a::reset();
        sda_b::reset();
        sda_c::reset();
        sda_d::reset();

        sda_a::init(hal::DigitalIO::Mode::OUTPUT);
        sda_b::init(hal::DigitalIO::Mode::OUTPUT);
        sda_c::init(hal::DigitalIO::Mode::OUTPUT);
        sda_d::init(hal::DigitalIO::Mode::OUTPUT);
    }

    static void sda_hi_z() {
        sda_a::init(hal::DigitalIO::Mode::INPUT);
        sda_b::init(hal::DigitalIO::Mode::INPUT);
        sda_c::init(hal::DigitalIO::Mode::INPUT);
        sda_d::init(hal::DigitalIO::Mode::INPUT);
    }

    static void wb(uint8_t& dest, uint8_t val, uint8_t pos) {
        if (val) {
            dest |= (val << pos);
        } else {
            dest &= ~(val << pos);
        }
    }

};
} // namespace SoftI2CMulti

#endif  // SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_I2C_MULTIPLE_H_
