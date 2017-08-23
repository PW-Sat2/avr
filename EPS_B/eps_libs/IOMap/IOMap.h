#ifndef EPS_B_EPS_LIBS_IOMAP_IOMAP_H_
#define EPS_B_EPS_LIBS_IOMAP_IOMAP_H_

#include <hal/hal>
#include "LclInterface.h"

namespace eps {

struct IOMap {
    using PowerCycle = hal::DigitalIO::GPIO<1>;
    using SerialRx   = hal::DigitalIO::GPIO<1>;

    struct watchdog {
        using PinKick  = hal::DigitalIO::GPIO<1>;
        using Watchdog = hal::devices::TPS3813<PinKick, 10>;
    };

    struct thermal_knives {
        using PinSail = hal::DigitalIO::GPIO<1>;
        using PinSads = hal::DigitalIO::GPIO<1>;
    };

    struct battery_controller {
        using PinHeater    = hal::DigitalIO::GPIO<1>;
        using PinCharge    = hal::DigitalIO::GPIO<1>;
        using PinDischarge = hal::DigitalIO::GPIO<1>;
    };


    struct lcl {
        struct ants {
            using PinOn    = hal::DigitalIO::GPIO<1>;
            using PinFlagB = hal::DigitalIO::GPIO<1>;
        };

        struct tk_red {
            using PinOn    = hal::DigitalIO::GPIO<1>;
            using PinFlagB = hal::DigitalIO::GPIO<1>;
        };


        enum class LclId : std::uint8_t {
            TkRed = 1,
            Ants  = 2,
        };

        // clang-format off
using TkRed = eps::LclInterface<tk_red::PinOn,
        tk_red::PinFlagB,
        LclId,
        LclId::TkRed,
        'T', 'K', 'r', 'e', 'd'>;

using Ants = eps::LclInterface<ants::PinOn,
        ants::PinFlagB,
        LclId,
        LclId::Ants,
        'A', 'n', 't', 's'>;
        // clang-format on

        using AllLcls = std::tuple<TkRed, Ants>;
    };
};

}  // namespace eps

#endif  // EPS_B_EPS_LIBS_IOMAP_IOMAP_H_
