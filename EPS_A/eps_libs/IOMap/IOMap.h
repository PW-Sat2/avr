#ifndef EPS_A_EPS_LIBS_IOMAP_IOMAP_H_
#define EPS_A_EPS_LIBS_IOMAP_IOMAP_H_

#include <hal/hal>

namespace eps_a {
namespace iomap {


namespace details {

template<int adc_cs_nr, int dac_cs_nr>
struct MpptChannel {
    using PinAdcCS = hal::DigitalIO::GPIO<adc_cs_nr>;
    using AdcSPI   = hal::SPI::Hardware<PinAdcCS,
                                      hal::SPI::HardwareClockDivisor::SPIHard_DIV_4,
                                      hal::SPI::Polarity::idle_low,
                                      hal::SPI::Phase::trailing_sample,
                                      hal::SPI::DataOrder::MSB_first>;

    using PinDacCS = hal::DigitalIO::GPIO<dac_cs_nr>;
    using DacSPI   = hal::SPI::Hardware<PinDacCS,
                                      hal::SPI::HardwareClockDivisor::SPIHard_DIV_4,
                                      hal::SPI::Polarity::idle_low,
                                      hal::SPI::Phase::leading_sample,
                                      hal::SPI::DataOrder::MSB_first>;

    using Adc124 = hal::devices::ADC124::ADC124<AdcSPI>;
    using Dac121 = hal::devices::DAC121::DAC121<DacSPI>;
};

template<typename PinOn, typename PinFlagB, std::uint8_t>
struct LCL
    : hal::devices::FPF270x<PinOn, PinFlagB, hal::DigitalIO::Dummy<false>> {};

}  // namespace details


using PowerCycle = hal::DigitalIO::GPIO<15>;
using SerialRx   = hal::DigitalIO::GPIO<2>;

namespace watchdog {
using PinKick  = hal::DigitalIO::GPIO<17>;
using Watchdog = hal::devices::TPS3813<PinKick, 10>;
}  // namespace watchdog

namespace mux {
using PinA0 = hal::DigitalIO::GPIO<8>;
using PinA1 = hal::DigitalIO::GPIO<9>;
using PinEN = hal::DigitalIO::GPIO<16>;

using Mux = hal::devices::ADG709::ADG709<PinA0, PinA1, PinEN>;
}  // namespace mux

namespace mppt {
using MpptX  = details::MpptChannel<6, 43>;
using MpptYp = details::MpptChannel<27, 28>;
using MpptYn = details::MpptChannel<31, 32>;
}  // namespace mppt

namespace thermal_knives {
using PinSail = hal::DigitalIO::GPIO<41>;
using PinSads = hal::DigitalIO::GPIO<42>;
}  // namespace thermal_knives


namespace battery_controller {
using PinHeater    = hal::DigitalIO::GPIO<34>;
using PinCharge    = hal::DigitalIO::GPIO<33>;
using PinDischarge = hal::DigitalIO::GPIO<14>;
}  // namespace battery_controller


namespace lcl {

namespace ants {
using PinOn    = hal::DigitalIO::GPIO<40>;
using PinFlagB = hal::DigitalIO::GPIO<46>;
}  // namespace ants

namespace sens {
using PinOn    = hal::DigitalIO::GPIO<39>;
using PinFlagB = hal::DigitalIO::GPIO<47>;
}  // namespace sens

namespace cam_wing {
using PinOn    = hal::DigitalIO::GPIO<38>;
using PinFlagB = hal::DigitalIO::GPIO<48>;
}  // namespace cam_wing

namespace cam_nadir {
using PinOn    = hal::DigitalIO::GPIO<37>;
using PinFlagB = hal::DigitalIO::GPIO<49>;
}  // namespace cam_nadir

namespace suns {
using PinOn    = hal::DigitalIO::GPIO<36>;
using PinFlagB = hal::DigitalIO::GPIO<50>;
}  // namespace suns

namespace tk_main {
using PinOn    = hal::DigitalIO::GPIO<35>;
using PinFlagB = hal::DigitalIO::GPIO<51>;
}  // namespace tk_main


enum class LclId : std::uint8_t {
    TkMain   = 1,
    Suns     = 2,
    CamNadir = 3,
    CamWing  = 4,
    Sens     = 5,
    Ants     = 6,
};

using TkMain = details::LCL<tk_main::PinOn,     //
                            tk_main::PinFlagB,  //
                            num(LclId::TkMain)>;

using Suns = details::LCL<suns::PinOn,     //
                          suns::PinFlagB,  //
                          num(LclId::Suns)>;

using CamNadir = details::LCL<cam_nadir::PinOn,     //
                              cam_nadir::PinFlagB,  //
                              num(LclId::CamNadir)>;

using CamWing = details::LCL<cam_wing::PinOn,     //
                             cam_wing::PinFlagB,  //
                             num(LclId::CamWing)>;

using Sens = details::LCL<sens::PinOn,     //
                          sens::PinFlagB,  //
                          num(LclId::Sens)>;

using Ants = details::LCL<ants::PinOn,     //
                          ants::PinFlagB,  //
                          num(LclId::Ants)>;

using AllLcls = std::tuple<TkMain, Suns, CamNadir, CamWing, Sens, Ants>;

}  // namespace lcl

}  // namespace iomap
}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_IOMAP_IOMAP_H_
