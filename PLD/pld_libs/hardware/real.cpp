#include <hardware/real.h>
#include <hal/hal>

using namespace hal;
using namespace devices;

namespace Mux {
using A0 = DigitalIO::GPIO<42>;
using A1 = DigitalIO::GPIO<40>;
using A2 = DigitalIO::GPIO<41>;
using EN = DigitalIO::GPIO<43>;
}  // namespace Mux

namespace adc128_interface {
using cs = DigitalIO::GPIO<9>;

using spi = SPI::Hardware<cs,
                          SPI::HardwareClockDivisor::SPIHard_DIV_16,
                          SPI::Polarity::idle_low,
                          SPI::Phase::trailing_sample,
                          SPI::DataOrder::MSB_first>;
}  // namespace adc128_interface

using adg708 = ADG708::ADG708<Mux::A0, Mux::A1, Mux::A2, Mux::EN>;
using adc128 = devices::ADC128::ADC128<adc128_interface::spi>;

using watchdog_pin = hal::DigitalIO::GPIO<44>;
using tps3813      = hal::devices::TPS3813<watchdog_pin, 10>;

using interrupt = hal::DigitalIO::GPIO<10>;


using ADCChannel = devices::ADC128::Channel;

constexpr static ADCChannel channel_to_adc_input_[] = {
    ADCChannel::IN0,  //    SunSRef_V0,
    ADCChannel::IN1,  //    SunSRef_V1,
    ADCChannel::IN2,  //    SunSRef_V2,
    ADCChannel::IN3,  //    SunSRef_V3,
    ADCChannel::IN4,  //    SunSRef_V4,
    ADCChannel::IN5,  //    Photodiode_A,
    ADCChannel::IN5,  //    Photodiode_B,
    ADCChannel::IN5,  //    Photodiode_C,
    ADCChannel::IN5,  //    Photodiode_D,
    ADCChannel::IN5,  //    TemperatureSupply,
    ADCChannel::IN5,  //    HouseKeeping_3V3d,
    ADCChannel::IN5,  //    HouseKeeping_3V3_OBC,
    ADCChannel::IN6,  //    TemperatureSail,
    ADCChannel::IN6,  //    TemperatureSADS,
    ADCChannel::IN6,  //    TemperatureCamWing,
    ADCChannel::IN6,  //    TemperatureCamNadir,
    ADCChannel::IN6,  //    TemperatureXn,
    ADCChannel::IN6,  //    TemperatureXp,
    ADCChannel::IN6,  //    TemperatureYn,
    ADCChannel::IN6   //    TemperatureYp,
};

constexpr static auto channel_to_adc_input(pld::hardware::AnalogChannel channel) {
    return channel_to_adc_input_[static_cast<int>(channel)];
}

using MuxChannel = ADG708::Channel;

constexpr static MuxChannel channel_to_mux_channel_[] = {
    MuxChannel::S1,  //    SunSRef_V0,
    MuxChannel::S1,  //    SunSRef_V1,
    MuxChannel::S1,  //    SunSRef_V2,
    MuxChannel::S1,  //    SunSRef_V3,
    MuxChannel::S1,  //    SunSRef_V4,

    MuxChannel::S5,  //    Photodiode_A,
    MuxChannel::S4,  //    Photodiode_B,
    MuxChannel::S8,  //    Photodiode_C,
    MuxChannel::S1,  //    Photodiode_D,
    MuxChannel::S6,  //    TemperatureSupply,
    MuxChannel::S3,  //    HouseKeeping_3V3d,
    MuxChannel::S2,  //    HouseKeeping_3V3_OBC,

    MuxChannel::S1,  //    TemperatureSail,
    MuxChannel::S8,  //    TemperatureSADS,
    MuxChannel::S2,  //    TemperatureCamWing,
    MuxChannel::S5,  //    TemperatureCamNadir,
    MuxChannel::S3,  //    TemperatureXn,
    MuxChannel::S6,  //    TemperatureXp,
    MuxChannel::S7,  //    TemperatureYn,
    MuxChannel::S4   //    TemperatureYp,
};

constexpr static auto channel_to_mux_channel(pld::hardware::AnalogChannel channel) {
    return channel_to_mux_channel_[static_cast<int>(channel)];
}

void pld::hardware::RealHardware::init() {
    interrupt::reset();
    interrupt::init(DigitalIO::Mode::OUTPUT);

    adc128_interface::spi::init();

    adg708::init();

    watchdog_pin::init(DigitalIO::Mode::OUTPUT);
}

void pld::hardware::RealHardware::read_adc(
    std::initializer_list<ChannelDescriptor> channels) {
    auto writer = channels.begin();

    adc128::read_and_change_channel(channel_to_adc_input(writer->channel));

    while (writer != channels.end() - 1) {
        adg708::select(channel_to_mux_channel(writer->channel));
        *writer->data = adc128::read_and_change_channel(
            channel_to_adc_input((writer + 1)->channel));
        writer++;
    }

    auto last = channels.end() - 1;
    adg708::select(channel_to_mux_channel(last->channel));
    *last->data =
        adc128::read_and_change_channel(channel_to_adc_input(last->channel));
}

pld::Telemetry::Radfet pld::hardware::RealHardware::read_radfet() {
    return pld::Telemetry::Radfet();
}

void pld::hardware::RealHardware::watchdog_kick() {
    hal::Watchdog::kick();
    tps3813::kick();
}

void pld::hardware::RealHardware::obc_interrupt_set() {
    interrupt::set();
}

void pld::hardware::RealHardware::obc_interrupt_reset() {
    interrupt::reset();
}
