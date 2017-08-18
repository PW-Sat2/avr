#ifndef EPS_B_EPS_LIBS_TELEMETRY_TELEMETRY_H_
#define EPS_B_EPS_LIBS_TELEMETRY_TELEMETRY_H_

#include <cstring>
#include "atomic.h"

namespace eps_b {

struct Telemetry {
    std::uint8_t who_am_i;

    void init() {
        std::memset(this, 0xFF, sizeof(*this));
    }
};
static_assert(sizeof(Telemetry) == 1,
              "Incorrect size of Telemetry structure (padding?)");
static_assert(std::is_pod<Telemetry>::value, "POD");


}  // namespace eps_b

#endif  // EPS_B_EPS_LIBS_TELEMETRY_TELEMETRY_H_
