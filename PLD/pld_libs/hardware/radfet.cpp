#include "hardware/IOMap.h"
#include "hardware/real.h"

using namespace hal;
using namespace std::chrono_literals;
using namespace radfet;
using namespace pld::devices;

namespace ADG849 {
using namespace hal::devices::ADG849;
}

namespace ADG709 {
using namespace hal::devices::ADG709;
}

enum class RadfetChannel : uint8_t {
    Temperature = 0,
    Vth0        = 1,
    Vth1        = 2,
    Vth2        = 3,
};

using RadfetConfig = std::tuple<ADG709::Channel,
                                ADG849::Channel,
                                pld::devices::AD7714::Channels,
                                pld::devices::AD7714::Gain>;

template<RadfetChannel>
RadfetConfig radfet_config = {0, 0, 0, 0};

template<>
RadfetConfig radfet_config<RadfetChannel::Temperature>{ADG709::Channel::S1,
                                                       ADG849::Channel::S1,
                                                       AD7714::Channels::AIN_3_4,
                                                       AD7714::Gain::GAIN_2};

template<>
RadfetConfig radfet_config<RadfetChannel::Vth0>{ADG709::Channel::S2,
                                                ADG849::Channel::S2,
                                                AD7714::Channels::AIN_3_4,
                                                AD7714::Gain::GAIN_1};

template<>
RadfetConfig radfet_config<RadfetChannel::Vth1>{ADG709::Channel::S3,
                                                ADG849::Channel::S2,
                                                AD7714::Channels::AIN_3_4,
                                                AD7714::Gain::GAIN_1};

template<>
RadfetConfig radfet_config<RadfetChannel::Vth2>{ADG709::Channel::S4,
                                                ADG849::Channel::S2,
                                                AD7714::Channels::AIN_3_4,
                                                AD7714::Gain::GAIN_1};


template<typename T, RadfetChannel channel>
auto config = std::get<T>(radfet_config<channel>);

template<RadfetChannel channel>
void read_channel(uint32_t& value, bool& timeout) {
    mux::high_side_current_mux::select(config<ADG849::Channel, channel>);
    mux::mos_mux::select(config<ADG709::Channel, channel>);

    hal::sleep_for(100ms);

    auto measurement = adc::ad7714::read(config<AD7714::Channels, channel>,
                                         config<AD7714::Gain, channel>);

    value   = measurement.value;
    timeout = measurement.timeout;
}


/*!
 * Commands
 */
void pld::hardware::RealHardware::radfet_on() {
    radfet::pin_lcl_3v3_overcurrent::init(hal::DigitalIO::Mode::INPUT);
    radfet::pin_lcl_5v_overcurrent::init(hal::DigitalIO::Mode::INPUT);

    radfet::pin_en::init(hal::DigitalIO::Mode::OUTPUT);
    radfet::pin_en::set();

    hal::sleep_for(100ms);

    radfet::adc::spi_soft::init();
    radfet::adc::ad7714::init();
    radfet::mux::mos_mux::init();
    radfet::mux::high_side_current_mux::init(ADG849::Channel::S2);
}

pld::hardware::Interface::RadfetMeasurement
pld::hardware::RealHardware::radfet_read() {
    pld::hardware::Interface::RadfetMeasurement data;

    radfet::mux::mos_mux::enable();

    read_channel<RadfetChannel::Vth0>(data.measurement.vth[0], data.timeout_vth0);
    read_channel<RadfetChannel::Vth1>(data.measurement.vth[1], data.timeout_vth1);
    read_channel<RadfetChannel::Vth2>(data.measurement.vth[2], data.timeout_vth2);
    read_channel<RadfetChannel::Temperature>(data.measurement.temperature,
                                             data.timeout_temperature);

    data.overcurrent_3v3 = false;
    data.overcurrent_5v  = false;
    for (auto i = 0; i < 100; ++i) {
        bool now_3v3 = !radfet::pin_lcl_3v3_overcurrent::read();
        bool now_5v  = !radfet::pin_lcl_5v_overcurrent::read();

        data.overcurrent_3v3 |= now_3v3;
        data.overcurrent_5v |= now_5v;

        hal::sleep_for(1ms);
    }

    radfet::mux::mos_mux::disable();

    return data;
}

template<typename T>
void deInit() {
    T::init(DigitalIO::Mode::INPUT);
}

void pld::hardware::RealHardware::radfet_off() {
    radfet::pin_en::init(DigitalIO::Mode::OUTPUT);
    radfet::pin_en::reset();


    deInit<radfet::mux::pin_en>();

    deInit<radfet::pin_lcl_3v3_overcurrent>();
    deInit<radfet::pin_lcl_5v_overcurrent>();

    deInit<mux::pin_en>();
    deInit<mux::pin_a0>();
    deInit<mux::pin_a1>();
    deInit<mux::pin_high_side_current_mux>();

    deInit<adc::pin_ss>();
    deInit<adc::pin_mosi>();
    deInit<adc::pin_miso>();
    deInit<adc::pin_sck>();
    deInit<adc::pin_drdy>();
}
