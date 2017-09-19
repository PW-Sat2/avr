#include "hardware/mock.h"

namespace suns {
namespace hardware {

using namespace mock;


void Mock::init() {
    this->obc_interrupt_reset();
}


void Mock::watchdog_kick() {
    hal::Watchdog::kick();
}

void Mock::als_measure(suns::Telemetry::Params,
                       suns::Telemetry::Status&,
                       suns::Telemetry::LightData&,
                       suns::Telemetry::LightData&) {
}

void Mock::temperatures_measure(suns::Telemetry::Temperatures&) {
}

void Mock::obc_interrupt_set() {
}

void Mock::obc_interrupt_reset() {
}

}  // namespace hardware
}  // namespace suns
