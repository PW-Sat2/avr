#ifndef EPS_B_EPS_LIBS_IOMAP_IOMAP_H_
#define EPS_B_EPS_LIBS_IOMAP_IOMAP_H_

#include <hal/hal>
#include "BatteryHeater.h"
#include "LclInterface.h"

namespace eps {

class IOMap {
 public:
    enum class LclId : std::uint8_t {
        ThermalKnives = 1,
        Ants          = 2,
    };

 private:
    template<int pin_on, int pin_flagb, LclId id, char... name>
    using Lcl =
        eps::LclInterface<hal::DigitalIO::GPIO<pin_on>, hal::DigitalIO::GPIO<pin_flagb>, LclId, id, name...>;

    using TkRed = Lcl<25, 31, LclId::ThermalKnives, 'T', 'K', 'r', 'e', 'd'>;
    using Ants  = Lcl<26, 30, LclId::Ants, 'A', 'n', 't', 's'>;

    using PinKick = hal::DigitalIO::GPIO<42>;

 public:
    // ~~~~~~~~~~~~~~~~ common for both EPS ~~~~~~~~~~~~~~~~

    using PowerCycle = hal::DigitalIO::GPIO<14>;
    using SerialRx   = hal::DigitalIO::GPIO<9>;

    using Watchdog = hal::devices::TPS3813<PinKick, 10>;

    struct ThermalKnives {
        using Sail = hal::DigitalIO::GPIO<15>;
        using Sads = hal::DigitalIO::GPIO<16>;
    };

    struct Battery {
        using Heater =
            eps::battery_heater::BatteryHeater<hal::DigitalIO::GPIO<12>,
                                               eps::battery_heater::LogicLevel::ActiveLow>;
        using Charge    = hal::DigitalIO::GPIO<11>;
        using Discharge = hal::DigitalIO::GPIO<13>;
    };

    using AllLcls = std::tuple<TkRed, Ants>;

    using PinsAsInputPullup = std::tuple<hal::DigitalIO::GPIO<1>,
                                         hal::DigitalIO::GPIO<2>,
                                         hal::DigitalIO::GPIO<3>,
                                         hal::DigitalIO::GPIO<9>,
                                         hal::DigitalIO::GPIO<10>,
                                         hal::DigitalIO::GPIO<19>,
                                         hal::DigitalIO::GPIO<20>,
                                         hal::DigitalIO::GPIO<33>,
                                         hal::DigitalIO::GPIO<40>,
                                         hal::DigitalIO::GPIO<41>,
                                         hal::DigitalIO::GPIO<43>,
                                         hal::DigitalIO::GPIO<44>>;
};

}  // namespace eps

#endif  // EPS_B_EPS_LIBS_IOMAP_IOMAP_H_
