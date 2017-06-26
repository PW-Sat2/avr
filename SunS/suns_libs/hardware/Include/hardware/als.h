#ifndef SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_ALS_H_
#define SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_ALS_H_

#include <hal/hal>

namespace BH1730FVCMulti {
enum class Gain : uint8_t {
    GAIN_1 = 0b00,
    GAIN_2 = 0b01,
    GAIN_64 = 0b10,
    GAIN_128 = 0b11,
};

template<typename i2c>
class BH1730FVCMulti : hal::libs::PureStatic {
 public:
    static void init() {
         i2c::init();
    }

    static uint8_t set_integration_time(uint8_t cycle) {
        uint8_t result = 0;
        result = i2c::start(address, SoftI2CMulti::Action::START_WRITE);
        if (result != 0) {
            return result;
        }

        result = i2c::write(COMMAND_REGISTER | TIMING);
        if (result != 0) {
            return result;
        }

        result = i2c::write(static_cast<uint8_t>(256-cycle));
        if (result != 0) {
            return result;
        }

        i2c::stop();
        return result;
    }


    static uint8_t set_gain(Gain gain) {
        uint8_t result = 0;

        result = i2c::start(address, SoftI2CMulti::Action::START_WRITE);
        if (result != 0) {
            return result;
        }

        result = i2c::write(COMMAND_REGISTER | GAIN);
        if (result != 0) {
            return result;
        }

        result = i2c::write(static_cast<uint8_t>(gain));
        if (result != 0) {
            return result;
        }

        i2c::stop();

        return result;
    }

    static uint8_t read_part_id(std::array<uint8_t, 4>& ids) {
        i2c::start(address, SoftI2CMulti::Action::START_WRITE);

        uint8_t result = i2c::write(COMMAND_REGISTER | ID);

        i2c::stop();

        i2c::start(address, SoftI2CMulti::Action::START_READ);

        ids = i2c::read(false);

        i2c::stop();

        return result;
    }

    static uint8_t read_ambient_light(std::array<uint16_t, 4>& VL, std::array<uint16_t, 4>& IR) {
        std::array<uint8_t, 4> DATA0MSB;
        std::array<uint8_t, 4> DATA0LSB;
        std::array<uint8_t, 4> DATA1MSB;
        std::array<uint8_t, 4> DATA1LSB;
        uint8_t result = 0;

        result = i2c::start(address, SoftI2CMulti::Action::START_WRITE);
        if (result != 0) {
            return result;
        }

        result = i2c::write(COMMAND_REGISTER | DATA0LOW);
        if (result != 0) {
            return result;
        }

        i2c::stop();

        result = i2c::start(address, SoftI2CMulti::Action::START_READ);
        if (result != 0) {
            return result;
        }

        DATA0LSB = i2c::read(true);
        DATA0MSB = i2c::read(true);
        DATA1LSB = i2c::read(true);
        DATA1MSB = i2c::read(false);

        i2c::stop();

        for (uint8_t i = 0; i < 4; i++) {
            VL[i] = (static_cast<uint16_t>(DATA0MSB[i]) << 8) | DATA0LSB[i];
            IR[i] = (static_cast<uint16_t>(DATA1MSB[i]) << 8) | DATA1LSB[i];
        }

        return result;
    }

    static uint8_t trigger() {
        uint8_t result = 0;

        result = i2c::start(address, SoftI2CMulti::Action::START_WRITE);
        if (result != 0) {
            return result;
        }

        result = i2c::write(COMMAND_REGISTER | CONTROL);
        if (result != 0) {
            return result;
        }

        result = i2c::write(ONE_SHOT | VL_IR | ADC_EN | ADC_POWER_ON);
        if (result != 0) {
            return result;
        }

        i2c::stop();
        return result;
    }


    static std::array<bool, 4> adc_valid() {
        i2c::start(address, SoftI2CMulti::Action::START_WRITE);

        i2c::write(COMMAND_REGISTER | CONTROL);

        i2c::stop();

        i2c::start(address, SoftI2CMulti::Action::START_READ);

        std::array<bool, 4> result;
        std::array<uint8_t, 4> control_registers = i2c::read(false);

        for(uint8_t i = 0; i < 4; i++) {
            result[i] = hal::libs::read_bit<4>(control_registers[i]);
        }

        i2c::stop();
        return result;
    }

 private:
    static constexpr uint8_t address = 0b0101001;

    enum RegisterAddr {
        CONTROL = 0x00,
        TIMING = 0x01,
        INTERRUPT = 0x02,
        THLLOW = 0x03,
        THLHIGH = 0x04,
        THHLOW = 0x05,
        THHHIGH = 0x06,
        GAIN = 0x07,
        ID = 0x12,
        DATA0LOW = 0x14,
        DATA0HIGH = 0x15,
        DATA1LOW = 0x16,
        DATA1HIGH = 0x17,
    };

    enum Commands {
        TRIGGER_MEASUREMENT = 0b10010100,
        COMMAND_REGISTER = 0b10000000,
        SPECIAL_COMMAND = 0b11000000,
        DATA_REGISTER = 0b0000000000,
        ADC_EN = 0b00000010,
        ADC_POWER_ON = 0b00000001,
    };

    enum MeasurementType {
        CONTINUOUS = 0b00000011,
        ONE_SHOT = 0b00001011,
    };

    enum DataSel {
        VL_IR = 0b00000000,
        VL_ONLY = 0b00000100,
    };
};

} // namespace BH1730FVCMulti

#endif  // SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_ALS_H_
