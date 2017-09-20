#include <hardware/als.h>
#include <hardware/i2c_multiple.h>
#include <hardware/real.h>
#include <hal/hal>
#include "logger.h"

using namespace hal;
using namespace devices;
using namespace libs;
using hal::Analog::AnalogGPIO;
using hal::Analog::InternalADC;

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
constexpr std::uint32_t als_timeout = 1600;  // 16000

namespace first {
using sda_a = hal::DigitalIO::GPIO<16>;
using sda_b = hal::DigitalIO::GPIO<11>;
using sda_c = hal::DigitalIO::GPIO<40>;
using sda_d = hal::DigitalIO::GPIO<30>;
using scl   = hal::DigitalIO::GPIO<3>;

using i2c =
    SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d, als_timeout>;
using als = BH1730FVCMulti::BH1730FVCMulti<i2c>;
}  // namespace first

namespace second {
using sda_a = hal::DigitalIO::GPIO<15>;
using sda_b = hal::DigitalIO::GPIO<2>;
using sda_c = hal::DigitalIO::GPIO<43>;
using sda_d = hal::DigitalIO::GPIO<31>;
using scl   = hal::DigitalIO::GPIO<41>;

using i2c =
    SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d, als_timeout>;
using als = BH1730FVCMulti::BH1730FVCMulti<i2c>;
}  // namespace second

namespace third {
using sda_a = hal::DigitalIO::GPIO<14>;
using sda_b = hal::DigitalIO::GPIO<1>;
using sda_c = hal::DigitalIO::GPIO<44>;
using sda_d = hal::DigitalIO::GPIO<32>;
using scl   = hal::DigitalIO::GPIO<13>;

using i2c =
    SoftI2CMulti::SoftI2CMulti<scl, sda_a, sda_b, sda_c, sda_d, als_timeout>;
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
void check_part_id(std::uint16_t& id_correctness, T& ids, const std::uint8_t offset) {
    constexpr std::uint8_t expected_id = 113;
    for (std::uint8_t i = 0; i < 4; i++) {
        if (expected_id == ids[i]) {
            set_bit(id_correctness, i + offset);
        }
    }
}

void part_id(suns::Telemetry::Status& status) {
    std::array<uint8_t, 4> ids;

    std::uint16_t ack_status = first::als::read_part_id(ids);
    status.ack |= ack_status;
    check_part_id(status.presence, ids, 0);

    ack_status = second::als::read_part_id(ids);
    status.ack |= (ack_status << 4);
    check_part_id(status.presence, ids, 4);

    ack_status = third::als::read_part_id(ids);
    status.ack |= (ack_status << 8);
    check_part_id(status.presence, ids, 8);
}

void set_itime(suns::Telemetry::Status& status, std::uint8_t itime) {
    std::uint16_t ack_status = first::als::set_integration_time(itime);
    status.ack |= ack_status;

    ack_status = second::als::set_integration_time(itime);
    status.ack |= (ack_status << 4);

    ack_status = third::als::set_integration_time(itime);
    status.ack |= (ack_status << 8);
}

void set_gain(suns::Telemetry::Status& status, BH1730FVCMulti::Gain gain) {
    std::uint16_t ack_status = first::als::set_gain(gain);
    status.ack |= ack_status;

    ack_status = second::als::set_gain(gain);
    status.ack |= (ack_status << 4);

    ack_status = third::als::set_gain(gain);
    status.ack |= (ack_status << 8);
}

void trigger(suns::Telemetry::Status& status) {
    std::uint16_t ack_status = first::als::trigger();
    status.ack |= ack_status;

    ack_status = second::als::trigger();
    status.ack |= (ack_status << 4);

    ack_status = third::als::trigger();
    status.ack |= (ack_status << 8);
}

void wait_expected_time(const std::uint8_t itime) {
    volatile std::uint8_t als_adc_conversion_wait        = 0;
    constexpr std::uint8_t adc_internal_calculation_time = 10;

    _delay_ms(adc_internal_calculation_time);

    while (als_adc_conversion_wait < itime) {
        _delay_us(2700);
        als_adc_conversion_wait++;
    }
}

void wait_for_als(suns::Telemetry::Status& status, const std::uint8_t itime) {
    constexpr std::uint8_t adc_valid          = 15;
    constexpr std::uint8_t adc_one_cycle_time = 3;
    volatile std::uint16_t als_timeout_cnt    = 0;

    std::uint16_t ack_status_1 = 0, ack_status_2 = 0, ack_status_3 = 0;
    std::uint8_t adc_state_1 = 0, adc_state_2 = 0, adc_state_3 = 0;

    wait_expected_time(itime);

    do {
        ack_status_1 |= first::als::adc_valid(adc_state_1);
        ack_status_2 |= second::als::adc_valid(adc_state_2);
        ack_status_3 |= third::als::adc_valid(adc_state_3);

        if (adc_valid == (adc_state_1 & adc_state_2 & adc_state_3)) {
            break;
        }
        _delay_ms(adc_one_cycle_time);
        als_timeout_cnt++;
    } while (als_timeout_cnt < itime);

    status.adc_valid = adc_state_1;
    status.adc_valid |= (adc_state_2 << 4);
    status.adc_valid |= (adc_state_3 << 8);

    status.ack |= ack_status_1;
    status.ack |= (ack_status_2 << 4);
    status.ack |= (ack_status_3 << 8);
}

void read_light(suns::Telemetry::Status& status,
                suns::Telemetry::LightData& vl,
                suns::Telemetry::LightData& ir) {
    std::uint16_t ack_status = first::als::read_ambient_light(vl.als_1, ir.als_1);
    status.ack |= ack_status;

    ack_status = second::als::read_ambient_light(vl.als_2, ir.als_2);
    status.ack |= (ack_status << 4);

    ack_status = third::als::read_ambient_light(vl.als_3, ir.als_3);
    status.ack |= (ack_status << 8);
}

}  // namespace all
}  // namespace als


void suns::hardware::RealHardware::init() {
    interrupt::reset();
    interrupt::init(hal::DigitalIO::Mode::OUTPUT);

    analog_pins::init();
    _delay_ms(10);
}

void suns::hardware::RealHardware::als_measure(suns::Telemetry::Params parameters,
                                               suns::Telemetry::Status& als_status,
                                               suns::Telemetry::LightData& vl,
                                               suns::Telemetry::LightData& ir) {
    als::all::init();

    als::all::part_id(als_status);

    als::all::set_itime(als_status, parameters.itime);
    als::all::set_gain(als_status,
                       static_cast<BH1730FVCMulti::Gain>(parameters.gain));
    als::all::trigger(als_status);
    als::all::wait_for_als(als_status, parameters.itime);
    als::all::read_light(als_status, vl, ir);
}

void suns::hardware::RealHardware::temperatures_measure(
    suns::Telemetry::Temperatures& temperature_data) {
    temperature_data.structure = AnalogGPIO<InternalADC::Input::ADC4>::read();
    temperature_data.panel_a   = AnalogGPIO<InternalADC::Input::ADC2>::read();
    temperature_data.panel_b   = AnalogGPIO<InternalADC::Input::ADC1>::read();
    temperature_data.panel_c   = AnalogGPIO<InternalADC::Input::ADC0>::read();
    temperature_data.panel_d   = AnalogGPIO<InternalADC::Input::ADC3>::read();
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
