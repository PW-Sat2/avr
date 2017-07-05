#include <hardware/real.h>
#include <hardware/i2c_multiple.h>
#include <hardware/als.h>
#include <hal/hal>

using namespace hal;
using namespace devices;
using namespace libs;
using hal::Analog::InternalADC;
using hal::Analog::AnalogGPIO;

using interrupt = hal::DigitalIO::GPIO<42>;

namespace analog_pins {
using adc_0 = hal::DigitalIO::GPIO<37>;
using adc_1 = hal::DigitalIO::GPIO<36>;
using adc_2 = hal::DigitalIO::GPIO<35>;
using adc_3 = hal::DigitalIO::GPIO<34>;
using adc_4 = hal::DigitalIO::GPIO<33>;

void init() {
    InternalADC::init(InternalADC::Prescaler::DIV_128,
                      InternalADC::Reference::AREF);

    adc_0::init(hal::DigitalIO::Mode::INPUT);
    adc_1::init(hal::DigitalIO::Mode::INPUT);
    adc_2::init(hal::DigitalIO::Mode::INPUT);
    adc_3::init(hal::DigitalIO::Mode::INPUT);
    adc_4::init(hal::DigitalIO::Mode::INPUT);
}
}  // namespace analog_pins

namespace als {
constexpr std::uint32_t als_timeout = 1600; //16000

namespace first {
using sda_a  = hal::DigitalIO::GPIO<16>;
using sda_b  = hal::DigitalIO::GPIO<11>;
using sda_c  = hal::DigitalIO::GPIO<40>;
using sda_d  = hal::DigitalIO::GPIO<30>;
using scl  = hal::DigitalIO::GPIO<3>;

using i2c = SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d, als_timeout>;
using als = BH1730FVCMulti::BH1730FVCMulti<i2c>;
}  // namespace first

namespace second {
using sda_a  = hal::DigitalIO::GPIO<15>;
using sda_b  = hal::DigitalIO::GPIO<2>;
using sda_c  = hal::DigitalIO::GPIO<43>;
using sda_d  = hal::DigitalIO::GPIO<31>;
using scl  = hal::DigitalIO::GPIO<41>;

using i2c = SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d, als_timeout>;
using als = BH1730FVCMulti::BH1730FVCMulti<i2c>;
}  // namespace second

namespace third {
using sda_a  = hal::DigitalIO::GPIO<14>;
using sda_b  = hal::DigitalIO::GPIO<1>;
using sda_c  = hal::DigitalIO::GPIO<44>;
using sda_d  = hal::DigitalIO::GPIO<32>;
using scl  = hal::DigitalIO::GPIO<13>;

using i2c = SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d, als_timeout>;
using als = BH1730FVCMulti::BH1730FVCMulti<i2c>;
}  // namespace third

namespace all {
void init() {
    first::i2c::init();
    second::i2c::init();
    third::i2c::init();

    first::als::init();
    second::als::init();
    third::als::init();
}

template<typename T>
void check_part_id(std::uint32_t& als_presense, T& ids, const std::uint8_t offset) {
    constexpr std::uint8_t expected_id = 113;
    for (std::uint8_t i = 0; i < 4; i++) {
        if (expected_id != ids[i]) {
            set_bit(als_presense, i + offset);
        }
    }
}

void part_id(std::uint32_t& status) {
    std::uint32_t temp_status = 0;
    std::array<uint8_t, 4> ids;

    temp_status = first::als::read_part_id(ids);
    write_mask<0, 5>(temp_status, read_mask<0, 4>(temp_status));
    check_part_id(temp_status, ids, 5);
    status |= temp_status;

    temp_status = second::als::read_part_id(ids);
    write_mask<9, 5>(temp_status, read_mask<0, 4>(temp_status));
    check_part_id(temp_status, ids, 14);
    status |= temp_status;

    temp_status = third::als::read_part_id(ids);
    write_mask<18, 5>(temp_status, read_mask<0, 4>(temp_status));
    check_part_id(temp_status, ids, 23);
    status |= temp_status;
}

void set_itime(std::uint32_t& status, std::uint8_t itime) {
    std::uint32_t temp_status = 0;

    temp_status = first::als::set_integration_time(itime);
    write_mask<0, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;

    temp_status = second::als::set_integration_time(itime);
    write_mask<9, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;

    temp_status = third::als::set_integration_time(itime);
    write_mask<18, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;
}

void set_gain(std::uint32_t& status, BH1730FVCMulti::Gain gain) {
    std::uint32_t temp_status = 0;

    temp_status = first::als::set_gain(gain);
    write_mask<0, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;

    temp_status = second::als::set_gain(gain);
    write_mask<9, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;

    temp_status = third::als::set_gain(gain);
    write_mask<18, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;
}

void trigger(std::uint32_t& status) {
    std::uint32_t temp_status = 0;

    temp_status = first::als::trigger();
    write_mask<0, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;

    temp_status = second::als::trigger();
    write_mask<9, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;

    temp_status = third::als::trigger();
    write_mask<18, 5>(temp_status, read_mask<0, 4>(temp_status));
    status |= temp_status;
}

void wait_for_als(std::uint32_t& status, const std::uint8_t itime) {
    constexpr std::uint8_t adc_valid = 15;
    constexpr std::uint8_t adc_one_cycle_time = 3;
    constexpr std::uint8_t adc_internal_calculation_time = 10;
    volatile std::uint8_t als_timeout_cnt = 0;

    std::uint32_t temp_status_1 = 0, temp_status_2 = 0, temp_status_3 = 0;
    std::uint8_t adc_state_1 = 0, adc_state_2 = 0, adc_state_3 = 0;

    _delay_ms(adc_internal_calculation_time);

    do {
        temp_status_1 |= first::als::adc_valid(adc_state_1);
        temp_status_2 |= second::als::adc_valid(adc_state_2);
        temp_status_3 |= third::als::adc_valid(adc_state_3);

        if (adc_valid == (adc_state_1 & adc_state_2 & adc_state_3)) {
            break;
        }

        _delay_ms(adc_one_cycle_time);
        als_timeout_cnt++;
    } while (als_timeout_cnt < itime);

    write_mask<0, 4>(temp_status_1, read_mask<0, 3>(temp_status_1));
    write_mask<8, 4>(temp_status_2, read_mask<0, 3>(temp_status_2));
    write_mask<16, 4>(temp_status_3, read_mask<0, 3>(temp_status_3));

    status |= temp_status_1;
    status |= temp_status_2;
    status |= temp_status_3;
}

void read_light(std::uint32_t& status, suns::Telemetry::LightData& vl, suns::Telemetry::LightData& ir) {

    std::uint32_t temp_status = 0;
    temp_status = first::als::read_ambient_light(vl.als_1, ir.als_1);
    write_mask<0, 4>(temp_status, read_mask<0, 3>(temp_status));
    status |= temp_status;

    temp_status = second::als::read_ambient_light(vl.als_2, ir.als_2);
    write_mask<8, 4>(temp_status, read_mask<0, 3>(temp_status));
    status |= temp_status;

    temp_status = third::als::read_ambient_light(vl.als_3, ir.als_3);
    write_mask<16, 4>(temp_status, read_mask<0, 3>(temp_status));
    status |= temp_status;
}

}  // namespace all
}  // namepsace als


void suns::hardware::RealHardware::init() {
    interrupt::reset();
    interrupt::init(hal::DigitalIO::Mode::OUTPUT);

    analog_pins::init();
    _delay_ms(10);
}

void suns::hardware::RealHardware::als_measure(std::uint8_t gain, std::uint8_t itime, std::uint32_t& status, suns::Telemetry::LightData& vl, suns::Telemetry::LightData& ir) {
    als::all::init();
    als::all::part_id(status);

    als::all::set_itime(status, itime);
    als::all::set_gain(status, static_cast<BH1730FVCMulti::Gain>(gain));
    als::all::trigger(status);
    als::all::wait_for_als(status, itime);
    als::all::read_light(status, vl, ir);
}

void suns::hardware::RealHardware::temperatures_measure(suns::Telemetry::Temperatures& temperature_data) {
    temperature_data.structure = AnalogGPIO<InternalADC::Input::ADC4>::read();
    temperature_data.panel_a = AnalogGPIO<InternalADC::Input::ADC2>::read();
    temperature_data.panel_b = AnalogGPIO<InternalADC::Input::ADC1>::read();
    temperature_data.panel_c = AnalogGPIO<InternalADC::Input::ADC0>::read();
    temperature_data.panel_d = AnalogGPIO<InternalADC::Input::ADC3>::read();
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
