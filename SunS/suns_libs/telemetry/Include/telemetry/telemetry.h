#ifndef SUNS_SUNS_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
#define SUNS_SUNS_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_

#include <util/atomic.h>
#include <array>
#include <cstdint>

namespace suns {
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
    struct Temperatures {
        std::uint16_t structure;
        std::uint16_t panel_a;
        std::uint16_t panel_b;
        std::uint16_t panel_c;
        std::uint16_t panel_d;
    };

    struct LightChannelData {
        std::uint16_t a;
        std::uint16_t b;
        std::uint16_t c;
        std::uint16_t d;
    };

    struct VL {
        LightChannelData als_1;
        LightChannelData als_2;
        LightChannelData als_3;
    };

    uint8_t who_am_i;
    uint32_t status;
    details::Atomic<VL> als_vl;
    details::Atomic<Temperatures> temperatures;
    details::Atomic<IR> als_ir;

    void init() {
        std::memset(this, 0xFF, sizeof(suns::Telemetry));
    }
};
static_assert(sizeof(Telemetry) == 65,
              "Incorrect size of Telemetry structure (padding?)");
static_assert(std::is_pod<Telemetry>::value, "POD");


}  // namespace suns

#endif  // SUNS_SUNS_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
