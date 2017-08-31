#ifndef EPS_B_EPS_LIBS_IOMAP_IOMAP_H_
#define EPS_B_EPS_LIBS_IOMAP_IOMAP_H_

#include <hal/hal>
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
        using Heater    = hal::DigitalIO::GPIO<12>;
        using Charge    = hal::DigitalIO::GPIO<11>;
        using Discharge = hal::DigitalIO::GPIO<13>;
    };

    using AllLcls = std::tuple<TkRed, Ants>;


    // ~~~~~~~~~~~~~~~~ EPS_A specific ~~~~~~~~~~~~~~~~
    using MpptXDisable  = hal::DigitalIO::GPIO<1>;
    using MpptYpDisable = hal::DigitalIO::GPIO<2>;
    using MpptYnDisable = hal::DigitalIO::GPIO<3>;
};

}  // namespace eps

#endif  // EPS_B_EPS_LIBS_IOMAP_IOMAP_H_
