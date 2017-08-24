#ifndef EPS_B_EPS_LIBS_IOMAP_IOMAP_H_
#define EPS_B_EPS_LIBS_IOMAP_IOMAP_H_

#include <hal/hal>
#include "LclInterface.h"

namespace eps {

class IOMap {
    enum class LclId : std::uint8_t {
        TkRed = 1,
        Ants  = 2,
    };

    template<int pin_on, int pin_flagb, LclId id, char... name>
    using Lcl =
        eps::LclInterface<hal::DigitalIO::GPIO<pin_on>, hal::DigitalIO::GPIO<pin_flagb>, LclId, id, name...>;

    using TkMain = Lcl<1, 1, LclId::TkRed, 'T', 'K', 'r', 'e', 'd'>;
    using Ants   = Lcl<1, 1, LclId::Ants, 'A', 'n', 't', 's'>;

    using PinKick = hal::DigitalIO::GPIO<1>;

 public:
    // ~~~~~~~~~~~~~~~~ common for both EPS ~~~~~~~~~~~~~~~~

    using PowerCycle = hal::DigitalIO::GPIO<1>;
    using SerialRx   = hal::DigitalIO::GPIO<1>;

    using Watchdog = hal::devices::TPS3813<PinKick, 10>;

    struct ThermalKnives {
        using Sail = hal::DigitalIO::GPIO<1>;
        using Sads = hal::DigitalIO::GPIO<1>;
    };

    struct Battery {
        using Heater    = hal::DigitalIO::GPIO<1>;
        using Charge    = hal::DigitalIO::GPIO<1>;
        using Discharge = hal::DigitalIO::GPIO<1>;
    };

    using AllLcls = std::tuple<TkMain, Ants>;


    // ~~~~~~~~~~~~~~~~ EPS_A specific ~~~~~~~~~~~~~~~~
};

}  // namespace eps

#endif  // EPS_B_EPS_LIBS_IOMAP_IOMAP_H_
