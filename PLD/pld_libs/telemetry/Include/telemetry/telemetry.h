#ifndef PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
#define PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_

#include <util/atomic.h>
#include <array>
#include <cstdint>
#include <cstring>

namespace pld {
namespace details {

template<class T>
class Atomic {
 public:
    void operator=(const T& rhs) {
        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            value = rhs;
        }
    }

    operator T() const {
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

    enum class RadfetState : std::uint8_t {
        TURNED_ON            = 1,
        TURNED_OFF           = 2,
        MEASUREMENT_EXECUTED = 3,
        ADC_TIMEOUT          = 4,
    };

    struct Radfet {
        RadfetState status;
        std::uint32_t temperature;
        std::array<std::uint32_t, 3> vth;
    };

    uint8_t who_am_i;
    details::Atomic<SunsRef> suns_ref;
    details::Atomic<Temperatures> temperatures;
    details::Atomic<Photodiodes> photodiodes;
    details::Atomic<Housekeeping> housekeeping;
    details::Atomic<Radfet> radfet;

    void init() {
        std::memset(this, 0xFF, sizeof(pld::Telemetry));
    }
};
static_assert(sizeof(Telemetry) == 58,
              "Incorrect size of Telemetry structure (padding?)");
static_assert(std::is_pod<Telemetry>::value, "POD");


}  // namespace pld

#endif  // PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
