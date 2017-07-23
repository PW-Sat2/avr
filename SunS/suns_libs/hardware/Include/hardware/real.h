#ifndef SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
#define SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_

#include <initializer_list>
#include "interface.h"
#include "telemetry/telemetry.h"

namespace suns {
namespace hardware {

struct RealHardware : public Interface {
    virtual void init() override;
    virtual void measure() override;
    virtual void watchdog_kick() override;
    virtual void obc_interrupt_set() override;
    virtual void obc_interrupt_reset() override;
};

}  // namespace hardware
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
