#include "hardware/real.h"
#include "Watchdog.h"
#include "hardware/IOMap.h"

using namespace hal;
using namespace devices;
using namespace std::chrono_literals;
using namespace pld::hardware;


using ADCChannel = devices::ADC128::Channel;

constexpr static ADCChannel channel_to_adc_input_[] = {
    ADCChannel::IN0,  //    SunSRef_V1
    ADCChannel::IN2,  //    SunSRef_V2
    ADCChannel::IN4,  //    SunSRef_V3
    ADCChannel::IN3,  //    SunSRef_V4
    ADCChannel::IN1,  //    SunSRef_V5

    ADCChannel::IN5,  //    TemperatureSupply
    ADCChannel::IN6,  //    TemperatureXp
    ADCChannel::IN6,  //    TemperatureXn
    ADCChannel::IN6,  //    TemperatureYp
    ADCChannel::IN6,  //    TemperatureYn
    ADCChannel::IN6,  //    TemperatureSADS
    ADCChannel::IN6,  //    TemperatureSail
    ADCChannel::IN6,  //    TemperatureCamNadir
    ADCChannel::IN6,  //    TemperatureCamWing

    ADCChannel::IN5,  //    PhotodiodeXp
    ADCChannel::IN5,  //    PhotodiodeXn
    ADCChannel::IN5,  //    PhotodiodeYp
    ADCChannel::IN5,  //    PhotodiodeYn

    ADCChannel::IN5,  //    HouseKeeping_3V3d
    ADCChannel::IN5   //    HouseKeeping_3V3_OBC
};

static_assert(
    sizeof(channel_to_adc_input_) / sizeof(ADCChannel) ==
        static_cast<size_t>(pld::hardware::AnalogChannel::LengthOfEnum),
    "Incorrect size of channel_to_adc_input_ array or AnalogChannel enum");

constexpr static auto channel_to_adc_input(pld::hardware::AnalogChannel channel) {
    return channel_to_adc_input_[static_cast<int>(channel)];
}

using MuxChannel = ADG708::Channel;

constexpr static MuxChannel channel_to_mux_channel_[] = {
    MuxChannel::S1,  //    SunSRef_V1
    MuxChannel::S1,  //    SunSRef_V2
    MuxChannel::S1,  //    SunSRef_V3
    MuxChannel::S1,  //    SunSRef_V4
    MuxChannel::S1,  //    SunSRef_V5

    MuxChannel::S6,  //    TemperatureSupply
    MuxChannel::S6,  //    TemperatureXp
    MuxChannel::S3,  //    TemperatureXn
    MuxChannel::S4,  //    TemperatureYp
    MuxChannel::S7,  //    TemperatureYn
    MuxChannel::S8,  //    TemperatureSADS
    MuxChannel::S1,  //    TemperatureSail
    MuxChannel::S5,  //    TemperatureCamNadir
    MuxChannel::S2,  //    TemperatureCamWing

    MuxChannel::S4,  //    PhotodiodeXp
    MuxChannel::S5,  //    PhotodiodeXn
    MuxChannel::S8,  //    PhotodiodeYp
    MuxChannel::S1,  //    PhotodiodeYn

    MuxChannel::S3,  //    HouseKeeping_3V3d
    MuxChannel::S2   //    HouseKeeping_3V3_OBC
};

static_assert(
    sizeof(channel_to_mux_channel_) / sizeof(MuxChannel) ==
        static_cast<size_t>(pld::hardware::AnalogChannel::LengthOfEnum),
    "Incorrect size of channel_to_mux_channel_ array or AnalogChannel enum");

constexpr static auto channel_to_mux_channel(pld::hardware::AnalogChannel channel) {
    return channel_to_mux_channel_[static_cast<int>(channel)];
}

void pld::hardware::RealHardware::init() {
    interrupt::reset();
    interrupt::init(DigitalIO::Mode::OUTPUT);

    adc128_interface::spi::init();

    adg708::init();

    external_watchdog::init();
}

void pld::hardware::RealHardware::read_adc(
    std::initializer_list<ChannelDescriptor> channels) {
    auto writer = channels.begin();
    adg708::enable();
    adc128::read_and_change_channel(channel_to_adc_input(writer->channel));

    while (writer != channels.end() - 1) {
        adg708::select(channel_to_mux_channel(writer->channel));
        hal::sleep_for(10ms);
        *writer->data = adc128::read_and_change_channel(
            channel_to_adc_input((writer + 1)->channel));
        writer++;
    }

    auto last = channels.end() - 1;
    adg708::select(channel_to_mux_channel(last->channel));
    hal::sleep_for(10ms);
    *last->data =
        adc128::read_and_change_channel(channel_to_adc_input(last->channel));
    adg708::disable();
}

void pld::hardware::RealHardware::external_watchdog_kick() {
    external_watchdog::kick();
}

void pld::hardware::RealHardware::obc_interrupt_set() {
    interrupt::set();
}

void pld::hardware::RealHardware::obc_interrupt_reset() {
    interrupt::reset();
}
