#ifndef PLD_SRC_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
#define PLD_SRC_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_

#include <array>
#include <cstdint>

namespace PLD {

struct Telemetry {
    struct SunsRef {
        std::array<std::uint16_t, 5> voltages;
    };

    struct Photodiodes {
        std::uint16_t X_p;
        std::uint16_t X_m;
        std::uint16_t Y_p;
        std::uint16_t Y_m;
    };

    struct Temperatures {
        std::uint16_t supply;
        std::uint16_t X_p;
        std::uint16_t X_m;
        std::uint16_t Y_p;
        std::uint16_t Y_m;
        std::uint16_t sads;
        std::uint16_t sail;
        std::uint16_t gyro;
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
static_assert(sizeof(Telemetry) == 55,
              "Incorrect size of Telemetry structure (padding?)");

extern Telemetry telemetry;

}  // namespace PLD

#endif  // PLD_SRC_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
