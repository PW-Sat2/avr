#ifndef SUNS_SUNS_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
#define SUNS_SUNS_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_

#include <util/atomic.h>
#include <array>
#include <cstdint>
#include <cstring>

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

    struct LightData {
        std::array<std::uint16_t, 4> als_1;
        std::array<std::uint16_t, 4> als_2;
        std::array<std::uint16_t, 4> als_3;
    };

    struct Params {
        uint8_t gain;
        uint8_t itime;
    };

    struct Status {
        uint16_t ack;
        uint16_t presence;
        uint16_t adc_valid;
    };

    uint8_t who_am_i;
    details::Atomic<Status> als_status;
    details::Atomic<LightData> vl_data;
    details::Atomic<Temperatures> temperature_data;
    details::Atomic<LightData> ir_data;
    details::Atomic<Params> parameters;

    void init() {
        std::memset(this, 0xFF, sizeof(suns::Telemetry));
    }
};
static_assert(sizeof(Telemetry) == 67,
              "Incorrect size of Telemetry structure (padding?)");
static_assert(std::is_pod<Telemetry>::value, "POD");


}  // namespace suns

#endif  // SUNS_SUNS_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
