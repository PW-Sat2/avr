#ifndef PLD_SRC_TELEMETRY_H_
#define PLD_SRC_TELEMETRY_H_

#include <array>
#include <cstdint>

#include <ObcInterface/ObcInterface.h>
#include <TelemetryBuffer/TelemetryBuffer.h>

namespace PLD {

struct Telemetry {
    struct SunsRef {
        std::array<std::uint16_t, 5> voltages;
    };

    struct Photodiodes {
        std::uint16_t Xp;
        std::uint16_t Xn;
        std::uint16_t Yp;
        std::uint16_t Yn;
    };

    struct Temperatures {
        std::uint16_t supply;
        std::uint16_t Xp;
        std::uint16_t Xn;
        std::uint16_t Yp;
        std::uint16_t Yn;
        std::uint16_t sads;
        std::uint16_t sail;
        std::uint16_t cam_nadir;
        std::uint16_t cam_wing;
    };

    struct Voltages {
        std::uint16_t int_3v3d;
        std::uint16_t obc_3v3d;
    };

    struct Radfet {
        std::uint32_t temperature;
        std::array<std::uint32_t, 3> vth;
    };

    uint8_t status;
    SunsRef suns_ref;
    Photodiodes photodiodes;
    Temperatures temperatures;
    Voltages voltages;
    Radfet radfet;
};
static_assert(sizeof(Telemetry) == 57,
              "Incorrect size of Telemetry structure (padding?)");


}  // namespace PLD

extern TelemetryBuffer<PLD::Telemetry> telemetryBuffer;

#endif  // PLD_SRC_TELEMETRY_H_
