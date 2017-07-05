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

void Mock::als_measure(uint8_t gain, uint8_t itime, std::uint32_t& status, suns::Telemetry::LightData& vl, suns::Telemetry::LightData& ir) {
    vl.als_1[0] = 1;
    ir.als_1[0] = 1;
    status = 10;
    printf("gain: %d, itime: %d\n", gain, itime);
}

void Mock::temperatures_measure(suns::Telemetry::Temperatures& temperature_data) {
    temperature_data.structure = 10;
}

void Mock::obc_interrupt_set() {

}

void Mock::obc_interrupt_reset() {

}

}  // namespace hardware
}  // namespace suns
