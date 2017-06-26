#include <hardware/real.h>
#include <hardware/i2c_multiple.h>
#include <hardware/als.h>
#include <hal/hal>

using namespace hal;
using namespace devices;

/*suns::Telemetry::LightData suns::hardware::RealHardware::read_als() {
    return suns::Telemetry::Radfet();
}*/

using interrupt = hal::DigitalIO::GPIO<10>;

namespace als {
namespace first {
using sda_a  = hal::DigitalIO::GPIO<16>;
using sda_b  = hal::DigitalIO::GPIO<11>;
using sda_c  = hal::DigitalIO::GPIO<40>;
using sda_d  = hal::DigitalIO::GPIO<30>;
using scl  = hal::DigitalIO::GPIO<3>;

using i2c = SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d>;
using als = BH1730FVCMulti::BH1730FVCMulti<i2c>;
}  // namespace first

namespace second {
using sda_a  = hal::DigitalIO::GPIO<15>;
using sda_b  = hal::DigitalIO::GPIO<2>;
using sda_c  = hal::DigitalIO::GPIO<43>;
using sda_d  = hal::DigitalIO::GPIO<31>;
using scl  = hal::DigitalIO::GPIO<41>;

using i2c = SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d>;
using als = BH1730FVCMulti::BH1730FVCMulti<i2c>;
}  // namespace second

namespace third {
using sda_a  = hal::DigitalIO::GPIO<14>;
using sda_b  = hal::DigitalIO::GPIO<1>;
using sda_c  = hal::DigitalIO::GPIO<44>;
using sda_d  = hal::DigitalIO::GPIO<32>;
using scl  = hal::DigitalIO::GPIO<13>;

using i2c = SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d>;
using als = BH1730FVCMulti::BH1730FVCMulti<i2c>;
}  // namespace third

void init() {
    first::i2c::init();
    second::i2c::init();
    third::i2c::init();

    first::als::init();
    second::als::init();
    third::als::init();
}

void check_part_id(std::uint16_t& als_presense, std::array<std::uint8_t, 4>& ids, const std::uint8_t pos) {
    constexpr uint8_t expected_id = 113;
    for (std::uint8_t i = 0; i < 4; i++) {
        if (expected_id != ids[i]) {
            hal::libs::reset_bit(als_presense, i + pos);
        }
    }
}

void part_id(std::uint16_t& i2c_status, std::uint16_t& als_presense) {
    std::uint8_t temp_status;
    std::array<uint8_t, 4> ids;

    temp_status = first::als::read_part_id(ids);
    check_part_id(als_presense, ids, 0);
    hal::libs::write_mask<0, 4>(i2c_status, hal::libs::read_mask<0, 4>(temp_status));

    temp_status = second::als::read_part_id(ids);
    check_part_id(als_presense, ids, 4);
    hal::libs::write_mask<4, 4>(i2c_status, hal::libs::read_mask<0, 4>(temp_status));

    temp_status = third::als::read_part_id(ids);
    check_part_id(als_presense, ids, 8);
    hal::libs::write_mask<0, 8>(i2c_status, hal::libs::read_mask<0, 4>(temp_status));
}

void 

}  // namepsace als


void suns::hardware::RealHardware::init() {
    _delay_ms(10);
}

suns::Telemetry::ALS suns::hardware::RealHardware::als_measure(uint8_t gain, uint8_t itime) {
    suns::Telemetry::ALS data;
    data.als_presense = 0b0000111111111111;
    data.als_i2c_status = 0b0000111111111111;
    
    als::init();
    als::part_id(data.als_i2c_status, data.als_presense);



    return data;
}

void suns::hardware::RealHardware::watchdog_kick() {
    hal::Watchdog::kick();
}

void suns::hardware::RealHardware::obc_interrupt_set() {
    interrupt::set();
}

void suns::hardware::RealHardware::obc_interrupt_reset() {
    interrupt::reset();
}
