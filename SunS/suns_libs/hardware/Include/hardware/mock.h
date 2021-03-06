#ifndef SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_
#define SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_

#include <hal/hal>
#include "interface.h"

namespace suns {
namespace hardware {

struct Mock : public Interface {
    virtual void init() override;
    virtual void als_measure(suns::Telemetry::Params parameters,
                             suns::Telemetry::Status& als_status,
                             suns::Telemetry::LightData& vl,
                             suns::Telemetry::LightData& ir) override;
    virtual void
    temperatures_measure(suns::Telemetry::Temperatures& temperature_data) override;
    void watchdog_kick() override;
    void obc_interrupt_set() override;
    void obc_interrupt_reset() override;
};

namespace mock {
enum class MockEvent : uint8_t {
    Init     = 0,
    Standby  = 1,
    Radfet   = 2,
    Watchdog = 3,
    IntSet   = 4,
    IntReset = 5,
};

}  // namespace mock
}  // namespace hardware
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_
