#include <hardware/real.h>
#include <hal/hal>

class Mux : public hal::devices::ADG708 {
 public:
    Mux() : ADG708(A0, A1, A2, EN) {
    }

 private:
    hal::DigitalIO::GPIO<42> A0;
    hal::DigitalIO::GPIO<40> A1;
    hal::DigitalIO::GPIO<41> A2;
    hal::DigitalIO::GPIO<43> EN;
};

class ADC128 : public hal::drivers::ADC128 {
 public:
    ADC128() : hal::drivers::ADC128(spi), spi(cs) {
    }

    void init() {
        spi.init();
    }

 private:
    hal::DigitalIO::GPIO<9> cs;

    hal::SPI::Hardware<hal::SPI::HardwareClockDivisor::SPIHard_DIV_4,
                       hal::SPI::Polarity::idle_high,
                       hal::SPI::Phase::trailing_sample,
                       hal::SPI::DataOrder::MSB_first>
        spi;
};

hal::DigitalIO::GPIO<44> watchdog_pin;
hal::devices::TPS3813<10> tps3813(watchdog_pin);

hal::DigitalIO::GPIO<10> interrupt;

static Mux mux;
static ADC128 adc128;

constexpr static hal::drivers::ADC128::Channel channel_to_adc_input_[] = {
    hal::drivers::ADC128::Channel::IN0,  //    SunSRef_V0,
    hal::drivers::ADC128::Channel::IN1,  //    SunSRef_V1,
    hal::drivers::ADC128::Channel::IN2,  //    SunSRef_V2,
    hal::drivers::ADC128::Channel::IN3,  //    SunSRef_V3,
    hal::drivers::ADC128::Channel::IN4,  //    SunSRef_V4,
    hal::drivers::ADC128::Channel::IN5,  //    Photodiode_A,
    hal::drivers::ADC128::Channel::IN5,  //    Photodiode_B,
    hal::drivers::ADC128::Channel::IN5,  //    Photodiode_C,
    hal::drivers::ADC128::Channel::IN5,  //    Photodiode_D,
    hal::drivers::ADC128::Channel::IN5,  //    TemperatureSupply,
    hal::drivers::ADC128::Channel::IN5,  //    HouseKeeping_3V3d,
    hal::drivers::ADC128::Channel::IN5,  //    HouseKeeping_3V3_OBC,
    hal::drivers::ADC128::Channel::IN6,  //    TemperatureSail,
    hal::drivers::ADC128::Channel::IN6,  //    TemperatureSADS,
    hal::drivers::ADC128::Channel::IN6,  //    TemperatureCamWing,
    hal::drivers::ADC128::Channel::IN6,  //    TemperatureCamNadir,
    hal::drivers::ADC128::Channel::IN6,  //    TemperatureXn,
    hal::drivers::ADC128::Channel::IN6,  //    TemperatureXp,
    hal::drivers::ADC128::Channel::IN6,  //    TemperatureYn,
    hal::drivers::ADC128::Channel::IN6   //    TemperatureYp,
};

constexpr static hal::drivers::ADC128::Channel
channel_to_adc_input(pld::hardware::AnalogChannel channel) {
    return channel_to_adc_input_[static_cast<int>(channel)];
}

using Channel = hal::devices::ADG708::Channel;

constexpr static hal::devices::ADG708::Channel channel_to_mux_channel_[] = {
    Channel::S1,  //    SunSRef_V0,
    Channel::S1,  //    SunSRef_V1,
    Channel::S1,  //    SunSRef_V2,
    Channel::S1,  //    SunSRef_V3,
    Channel::S1,  //    SunSRef_V4,

    Channel::S5,  //    Photodiode_A,
    Channel::S4,  //    Photodiode_B,
    Channel::S8,  //    Photodiode_C,
    Channel::S1,  //    Photodiode_D,
    Channel::S6,  //    TemperatureSupply,
    Channel::S3,  //    HouseKeeping_3V3d,
    Channel::S2,  //    HouseKeeping_3V3_OBC,

    Channel::S1,  //    TemperatureSail,
    Channel::S8,  //    TemperatureSADS,
    Channel::S2,  //    TemperatureCamWing,
    Channel::S5,  //    TemperatureCamNadir,
    Channel::S3,  //    TemperatureXn,
    Channel::S6,  //    TemperatureXp,
    Channel::S7,  //    TemperatureYn,
    Channel::S4   //    TemperatureYp,
};

constexpr static hal::devices::ADG708::Channel
channel_to_mux_channel(pld::hardware::AnalogChannel channel) {
    return channel_to_mux_channel_[static_cast<int>(channel)];
}

void pld::hardware::RealHardware::init() {
    adc128.init();
    mux.init();
    mux.enable();
    tps3813.init();
}

void pld::hardware::RealHardware::read_adc(
    std::initializer_list<ChannelDescriptor> channels) {
    auto writer = channels.begin();

    adc128.read_and_change_channel(channel_to_adc_input(writer->channel));

    while (writer != channels.end() - 1) {
        mux.select(channel_to_mux_channel(writer->channel));
        *writer->data = adc128.read_and_change_channel(
            channel_to_adc_input((writer + 1)->channel));
        writer++;
    }

    auto last = channels.end() - 1;
    mux.select(channel_to_mux_channel(last->channel));
    *last->data =
        adc128.read_and_change_channel(channel_to_adc_input(last->channel));
}

pld::Telemetry::Radfet pld::hardware::RealHardware::read_radfet() {
    return pld::Telemetry::Radfet();
}

void pld::hardware::RealHardware::watchdog_kick() {
    hal::Watchdog::kick();
    tps3813.kick();
}

void pld::hardware::RealHardware::obc_interrupt_set() {
    interrupt.set();
}

void pld::hardware::RealHardware::obc_interrupt_reset() {
    interrupt.reset();
}
