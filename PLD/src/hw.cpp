#include "hw.h"

constexpr static hal::drivers::ADC128::Channel channel_to_adc_input[] = {
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

void PLDHardware::init() {
    this->mux.init();
    this->adc128.init();
    this->radFET.init();
}

void PLDHardware::standby() {
    mux.disable();
}

uint16_t PLDHardware::read_adc_and_change_channel(AnalogChannel next_channel) {
    uint8_t channel = static_cast<std::uint8_t>(next_channel);

    static uint8_t prev = 0;
    printf("Reading channel %d\n", prev);
    prev = channel;

    return this->adc128.readAndChangeChannel(channel_to_adc_input[channel]);
}
