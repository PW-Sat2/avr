#ifndef EPS_A_EPS_LIBS_IOMAP_IOMAP_H_
#define EPS_A_EPS_LIBS_IOMAP_IOMAP_H_

#include <hal/hal>
#include "BatteryHeater.h"
#include "LclInterface.h"

namespace eps {

class IOMap {
 public:
    enum class LclId : std::uint8_t {
        ThermalKnives = 1,
        Suns          = 2,
        CamNadir      = 3,
        CamWing       = 4,
        Sens          = 5,
        Ants          = 6,
    };

 private:
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

    template<int cs_nr>
    struct TMP121Sensors {
        using PinCs = hal::DigitalIO::GPIO<cs_nr>;
        using Spi =
            hal::SPI::Hardware<PinCs,
                               hal::SPI::HardwareClockDivisor::SPIHard_DIV_16,
                               hal::SPI::Polarity::idle_high,
                               hal::SPI::Phase::leading_sample,
                               hal::SPI::DataOrder::MSB_first>;

        using Tmp121 = hal::devices::TMP121<Spi>;
    };

    template<int pin_on, int pin_flagb, LclId id, char... name>
    using Lcl =
        eps::LclInterface<hal::DigitalIO::GPIO<pin_on>, hal::DigitalIO::GPIO<pin_flagb>, LclId, id, name...>;

    // clang-format off
    using TkMain   = Lcl<35, 51, LclId::ThermalKnives, 'T', 'K', 'm', 'a', 'i', 'n'>;
    using Suns     = Lcl<36, 50, LclId::Suns,          'S', 'u', 'n', 'S'>;
    using CamNadir = Lcl<37, 49, LclId::CamNadir,      'C', 'a', 'm', 'N', 'a', 'd', 'i', 'r'>;
    using CamWing  = Lcl<38, 48, LclId::CamWing,       'C', 'a', 'm', 'W', 'i', 'n', 'g'>;
    using Sens     = Lcl<39, 47, LclId::Sens,          'S', 'e', 'n', 's'>;
    using Ants     = Lcl<40, 46, LclId::Ants,          'A', 'n', 't', 's'>;
    // clang-format on

    using PinKick = hal::DigitalIO::GPIO<17>;

    struct mux {
        using PinA0 = hal::DigitalIO::GPIO<8>;
        using PinA1 = hal::DigitalIO::GPIO<9>;
        using PinEN = hal::DigitalIO::GPIO<16>;
    };

 public:
    // ~~~~~~~~~~~~~~~~ common for both EPS ~~~~~~~~~~~~~~~~

    using PowerCycle = hal::DigitalIO::GPIO<15>;
    using SerialRx   = hal::DigitalIO::GPIO<2>;

    using Watchdog = hal::devices::TPS3813<PinKick, 10>;

    struct ThermalKnives {
        using Sail = hal::DigitalIO::GPIO<41>;
        using Sads = hal::DigitalIO::GPIO<42>;
    };

    struct Battery {
        using Heater =
            eps::battery_heater::BatteryHeater<hal::DigitalIO::GPIO<34>,
                                               eps::battery_heater::LogicLevel::ActiveHigh>;
        using Charge    = hal::DigitalIO::GPIO<33>;
        using Discharge = hal::DigitalIO::GPIO<14>;
    };

    using AllLcls = std::tuple<TkMain, Suns, CamNadir, CamWing, Sens, Ants>;


    // ~~~~~~~~~~~~~~~~ EPS_A specific ~~~~~~~~~~~~~~~~

    using Mux = hal::devices::ADG709::ADG709<mux::PinA0, mux::PinA1, mux::PinEN>;

    struct Mppt {
        using MpptX  = MpptChannel<6, 43>;
        using MpptYp = MpptChannel<27, 28>;
        using MpptYn = MpptChannel<31, 32>;
    };

    using BatteryTemperatureSensorA = TMP121Sensors<29>;
    using BatteryTemperatureSensorB = TMP121Sensors<30>;
};

}  // namespace eps

#endif  // EPS_A_EPS_LIBS_IOMAP_IOMAP_H_
