#ifndef PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
#define PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_

#include <util/atomic.h>
#include <array>
#include <cstdint>

namespace pld {

namespace details {


template<class T>
class Atomic {
 public:
    T& operator=(const T& rhs) {
        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            value = rhs;
        }
        return value;
    }

    operator T() {
        return value;
    }

 private:
    T value;
};


}  // namespace details

struct Telemetry {
    struct SunsRef {
        std::array<std::uint16_t, 5> voltages;
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

    struct Photodiodes {
        std::uint16_t Xp;
        std::uint16_t Xn;
        std::uint16_t Yp;
        std::uint16_t Yn;
    };

    struct Housekeeping {
        std::uint16_t int_3v3d;
        std::uint16_t obc_3v3d;
    };

    struct Radfet {
        std::uint32_t temperature;
        std::array<std::uint32_t, 3> vth;
    };

    details::Atomic<SunsRef> suns_ref;
    details::Atomic<Temperatures> temperatures;
    details::Atomic<Photodiodes> photodiodes;
    details::Atomic<Housekeeping> housekeeping;
    details::Atomic<Radfet> radfet;
};
static_assert(sizeof(Telemetry) == 56,
              "Incorrect size of Telemetry structure (padding?)");
static_assert(std::is_pod<Telemetry>::value, "POD");


}  // namespace pld

#endif  // PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
