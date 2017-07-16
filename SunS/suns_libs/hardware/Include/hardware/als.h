#ifndef SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_ALS_H_
#define SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_ALS_H_

#include <hal/hal>

namespace BH1730FVCMulti {
enum class Gain : std::uint8_t {
    GAIN_1   = 0b00,
    GAIN_2   = 0b01,
    GAIN_64  = 0b10,
    GAIN_128 = 0b11,
};

template<typename i2c>
class BH1730FVCMulti : hal::libs::PureStatic {
 public:
    static void init() {
        i2c::init();
    }

    static std::uint8_t set_integration_time(std::uint8_t cycle) {
        std::uint8_t result =
            i2c::start(address, SoftI2CMulti::Action::START_WRITE);

        result |= i2c::write(COMMAND_REGISTER | TIMING);

        result |= i2c::write(static_cast<std::uint8_t>(256 - cycle));

        i2c::stop();
        return result;
    }


    static std::uint8_t set_gain(Gain gain) {
        std::uint8_t result =
            i2c::start(address, SoftI2CMulti::Action::START_WRITE);

        result |= i2c::write(COMMAND_REGISTER | GAIN);

        result |= i2c::write(static_cast<std::uint8_t>(gain));

        i2c::stop();

        return result;
    }

    static std::uint8_t read_part_id(std::array<std::uint8_t, 4>& ids) {
        std::uint8_t result =
            i2c::start(address, SoftI2CMulti::Action::START_WRITE);

        result |= i2c::write(COMMAND_REGISTER | ID);

        i2c::stop();

        result |= i2c::start(address, SoftI2CMulti::Action::START_READ);
        result |= i2c::read(ids, false);

        i2c::stop();

        return result;
    }

    static std::uint8_t read_ambient_light(std::array<std::uint16_t, 4>& VL,
                                           std::array<std::uint16_t, 4>& IR) {
        std::array<std::uint8_t, 4> vl_msb, vl_lsb, ir_msb, ir_lsb;

        std::uint8_t result =
            i2c::start(address, SoftI2CMulti::Action::START_WRITE);

        result |= i2c::write(COMMAND_REGISTER | DATA0LOW);

        i2c::stop();

        result |= i2c::start(address, SoftI2CMulti::Action::START_READ);

        result = i2c::read(vl_lsb, true);
        result = i2c::read(vl_msb, true);
        result = i2c::read(ir_lsb, true);
        result = i2c::read(ir_msb, false);

        i2c::stop();

        for (uint8_t i = 0; i < 4; i++) {
            hal::libs::write_mask<0, 8>(VL[i], vl_lsb[i]);
            hal::libs::write_mask<8, 8>(VL[i], vl_msb[i]);
            hal::libs::write_mask<0, 8>(IR[i], ir_lsb[i]);
            hal::libs::write_mask<8, 8>(IR[i], ir_msb[i]);
        }

        return result;
    }

    static std::uint8_t trigger() {
        std::uint8_t result =
            i2c::start(address, SoftI2CMulti::Action::START_WRITE);

        result |= i2c::write(COMMAND_REGISTER | CONTROL);

        result |= i2c::write(ONE_SHOT | VL_IR | ADC_EN | ADC_POWER_ON);

        i2c::stop();
        return result;
    }


    static std::uint8_t adc_valid(std::uint8_t& data_status) {
        std::uint8_t result =
            i2c::start(address, SoftI2CMulti::Action::START_WRITE);

        result |= i2c::write(COMMAND_REGISTER | CONTROL);

        i2c::stop();

        result |= i2c::start(address, SoftI2CMulti::Action::START_READ);

        std::array<std::uint8_t, 4> control_registers;
        result = i2c::read(control_registers, false);

        hal::libs::write_bit<0>(data_status,
                                hal::libs::read_bit<4>(control_registers[0]));
        hal::libs::write_bit<1>(data_status,
                                hal::libs::read_bit<4>(control_registers[1]));
        hal::libs::write_bit<2>(data_status,
                                hal::libs::read_bit<4>(control_registers[2]));
        hal::libs::write_bit<3>(data_status,
                                hal::libs::read_bit<4>(control_registers[3]));

        i2c::stop();
        return result;
    }

 private:
    static constexpr std::uint8_t address = 0b0101001;

    enum RegisterAddr {
        CONTROL   = 0x00,
        TIMING    = 0x01,
        INTERRUPT = 0x02,
        THLLOW    = 0x03,
        THLHIGH   = 0x04,
        THHLOW    = 0x05,
        THHHIGH   = 0x06,
        GAIN      = 0x07,
        ID        = 0x12,
        DATA0LOW  = 0x14,
        DATA0HIGH = 0x15,
        DATA1LOW  = 0x16,
        DATA1HIGH = 0x17,
    };

    enum Commands {
        TRIGGER_MEASUREMENT = 0b10010100,
        COMMAND_REGISTER    = 0b10000000,
        SPECIAL_COMMAND     = 0b11100000,
        DATA_REGISTER       = 0b0000000000,
        ADC_EN              = 0b00000010,
        ADC_POWER_ON        = 0b00000001,
    };

    enum MeasurementType {
        CONTINUOUS = 0b00000011,
        ONE_SHOT   = 0b00001011,
    };

    enum DataSel {
        VL_IR   = 0b00000000,
        VL_ONLY = 0b00000100,
    };
};

}  // namespace BH1730FVCMulti

#endif  // SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_ALS_H_
