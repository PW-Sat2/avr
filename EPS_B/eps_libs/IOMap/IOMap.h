#ifndef EPS_A_EPS_LIBS_IOMAP_IOMAP_H_
#define EPS_A_EPS_LIBS_IOMAP_IOMAP_H_

#include <hal/hal>
#include "LclInterface.h"

namespace eps_a {
namespace iomap {


namespace details {

template<int adc_cs_nr, int dac_cs_nr>
struct MpptChannel {
    using PinAdcCS = hal::DigitalIO::GPIO<adc_cs_nr>;
    using AdcSpi   = hal::SPI::Hardware<PinAdcCS,
                                      hal::SPI::HardwareClockDivisor::SPIHard_DIV_4,
                                      hal::SPI::Polarity::idle_low,
                                      hal::SPI::Phase::trailing_sample,
                                      hal::SPI::DataOrder::MSB_first>;

    using PinDacCS = hal::DigitalIO::GPIO<dac_cs_nr>;
    using DacSpi   = hal::SPI::Hardware<PinDacCS,
                                      hal::SPI::HardwareClockDivisor::SPIHard_DIV_4,
                                      hal::SPI::Polarity::idle_low,
                                      hal::SPI::Phase::leading_sample,
                                      hal::SPI::DataOrder::MSB_first>;

    using Adc124 = hal::devices::ADC124::ADC124<AdcSpi>;
    using Dac121 = hal::devices::DAC121::DAC121<DacSpi>;
};
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

struct Mppt {
    using MpptX  = details::MpptChannel<6, 43>;
    using MpptYp = details::MpptChannel<27, 28>;
    using MpptYn = details::MpptChannel<31, 32>;
};

namespace thermal_knives {
using PinSail = hal::DigitalIO::GPIO<41>;
using PinSads = hal::DigitalIO::GPIO<42>;
}  // namespace thermal_knives


namespace battery_controller {
using PinHeater    = hal::DigitalIO::GPIO<34>;
using PinCharge    = hal::DigitalIO::GPIO<33>;
using PinDischarge = hal::DigitalIO::GPIO<14>;

template<int cs_nr>
struct BatSensor {
    using PinCs = hal::DigitalIO::GPIO<cs_nr>;
    using Spi   = hal::SPI::Hardware<PinCs,
                                   hal::SPI::HardwareClockDivisor::SPIHard_DIV_16,
                                   hal::SPI::Polarity::idle_low,
                                   hal::SPI::Phase::leading_sample,
                                   hal::SPI::DataOrder::MSB_first>;

    using Tmp121 = hal::devices::TMP121<Spi>;
};
struct TemperatureSensors {
    using TemperatureSensorA = BatSensor<29>;
    using TemperatureSensorB = BatSensor<30>;
};
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

// clang-format off
using TkMain = eps::LclInterface<tk_main::PinOn,
                        tk_main::PinFlagB,
                        LclId,
                        LclId::TkMain,
                        'T', 'K', 'm', 'a', 'i', 'n'>;

using Suns = eps::LclInterface<suns::PinOn,
                      suns::PinFlagB,
                      LclId,
                      LclId::Suns,
                      'S', 'u', 'n', 'S'>;

using CamNadir = eps::LclInterface<cam_nadir::PinOn,
                          cam_nadir::PinFlagB,
                          LclId,
                          LclId::CamNadir,
                          'C', 'a', 'm', 'N', 'a', 'd', 'i', 'r'>;

using CamWing = eps::LclInterface<cam_wing::PinOn,
                         cam_wing::PinFlagB,
                         LclId,
                         LclId::CamWing,
                         'C', 'a', 'm', 'W', 'i', 'n', 'g'>;

using Sens = eps::LclInterface<sens::PinOn,
                      sens::PinFlagB,
                      LclId,
                      LclId::Sens,
                      'S', 'e', 'n', 's'>;

using Ants = eps::LclInterface<ants::PinOn,
                      ants::PinFlagB,
                      LclId,
                      LclId::Ants,
                      'A', 'n', 't', 's'>;
// clang-format on

using AllLcls = std::tuple<TkMain, Suns, CamNadir, CamWing, Sens, Ants>;

}  // namespace lcl

}  // namespace iomap

struct IOMap {
    struct details {
        template<int adc_cs_nr, int dac_cs_nr>
        struct MpptChannel {
            using PinAdcCS = hal::DigitalIO::GPIO<adc_cs_nr>;
            using AdcSpi =
                hal::SPI::Hardware<PinAdcCS,
                                   hal::SPI::HardwareClockDivisor::SPIHard_DIV_4,
                                   hal::SPI::Polarity::idle_low,
                                   hal::SPI::Phase::trailing_sample,
                                   hal::SPI::DataOrder::MSB_first>;

            using PinDacCS = hal::DigitalIO::GPIO<dac_cs_nr>;
            using DacSpi =
                hal::SPI::Hardware<PinDacCS,
                                   hal::SPI::HardwareClockDivisor::SPIHard_DIV_4,
                                   hal::SPI::Polarity::idle_low,
                                   hal::SPI::Phase::leading_sample,
                                   hal::SPI::DataOrder::MSB_first>;

            using Adc124 = hal::devices::ADC124::ADC124<AdcSpi>;
            using Dac121 = hal::devices::DAC121::DAC121<DacSpi>;
        };
    };


    using PowerCycle = hal::DigitalIO::GPIO<15>;
    using SerialRx   = hal::DigitalIO::GPIO<2>;

    struct watchdog {
        using PinKick  = hal::DigitalIO::GPIO<17>;
        using Watchdog = hal::devices::TPS3813<PinKick, 10>;
    };

    struct mux {
        using PinA0 = hal::DigitalIO::GPIO<8>;
        using PinA1 = hal::DigitalIO::GPIO<9>;
        using PinEN = hal::DigitalIO::GPIO<16>;
    };
    using Mux = hal::devices::ADG709::ADG709<mux::PinA0, mux::PinA1, mux::PinEN>;

    struct Mppt {
        using MpptX  = details::MpptChannel<6, 43>;
        using MpptYp = details::MpptChannel<27, 28>;
        using MpptYn = details::MpptChannel<31, 32>;
    };

    struct thermal_knives {
        using PinSail = hal::DigitalIO::GPIO<41>;
        using PinSads = hal::DigitalIO::GPIO<42>;
    };


    struct battery_controller {
        using PinHeater    = hal::DigitalIO::GPIO<34>;
        using PinCharge    = hal::DigitalIO::GPIO<33>;
        using PinDischarge = hal::DigitalIO::GPIO<14>;

        template<int cs_nr>
        struct BatSensor {
            using PinCs = hal::DigitalIO::GPIO<cs_nr>;
            using Spi =
                hal::SPI::Hardware<PinCs,
                                   hal::SPI::HardwareClockDivisor::SPIHard_DIV_16,
                                   hal::SPI::Polarity::idle_low,
                                   hal::SPI::Phase::leading_sample,
                                   hal::SPI::DataOrder::MSB_first>;

            using Tmp121 = hal::devices::TMP121<Spi>;
        };
        struct TemperatureSensors {
            using TemperatureSensorA = BatSensor<29>;
            using TemperatureSensorB = BatSensor<30>;
        };
    };


    struct lcl {
        struct ants {
            using PinOn    = hal::DigitalIO::GPIO<40>;
            using PinFlagB = hal::DigitalIO::GPIO<46>;
        };

        struct sens {
            using PinOn    = hal::DigitalIO::GPIO<39>;
            using PinFlagB = hal::DigitalIO::GPIO<47>;
        };

        struct cam_wing {
            using PinOn    = hal::DigitalIO::GPIO<38>;
            using PinFlagB = hal::DigitalIO::GPIO<48>;
        };

        struct cam_nadir {
            using PinOn    = hal::DigitalIO::GPIO<37>;
            using PinFlagB = hal::DigitalIO::GPIO<49>;
        };

        struct suns {
            using PinOn    = hal::DigitalIO::GPIO<36>;
            using PinFlagB = hal::DigitalIO::GPIO<50>;
        };

        struct tk_main {
            using PinOn    = hal::DigitalIO::GPIO<35>;
            using PinFlagB = hal::DigitalIO::GPIO<51>;
        };


        enum class LclId : std::uint8_t {
            TkMain   = 1,
            Suns     = 2,
            CamNadir = 3,
            CamWing  = 4,
            Sens     = 5,
            Ants     = 6,
        };

        // clang-format off
using TkMain = eps::LclInterface<tk_main::PinOn,
        tk_main::PinFlagB,
        LclId,
        LclId::TkMain,
        'T', 'K', 'm', 'a', 'i', 'n'>;

using Suns = eps::LclInterface<suns::PinOn,
        suns::PinFlagB,
        LclId,
        LclId::Suns,
        'S', 'u', 'n', 'S'>;

using CamNadir = eps::LclInterface<cam_nadir::PinOn,
        cam_nadir::PinFlagB,
        LclId,
        LclId::CamNadir,
        'C', 'a', 'm', 'N', 'a', 'd', 'i', 'r'>;

using CamWing = eps::LclInterface<cam_wing::PinOn,
        cam_wing::PinFlagB,
        LclId,
        LclId::CamWing,
        'C', 'a', 'm', 'W', 'i', 'n', 'g'>;

using Sens = eps::LclInterface<sens::PinOn,
        sens::PinFlagB,
        LclId,
        LclId::Sens,
        'S', 'e', 'n', 's'>;

using Ants = eps::LclInterface<ants::PinOn,
        ants::PinFlagB,
        LclId,
        LclId::Ants,
        'A', 'n', 't', 's'>;
        // clang-format on

        using AllLcls = std::tuple<TkMain, Suns, CamNadir, CamWing, Sens, Ants>;
    };
};

}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_IOMAP_IOMAP_H_
