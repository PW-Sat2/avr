#ifndef EPS_B_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_
#define EPS_B_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_

#include <hal/hal>
#include "PowerCycleCounter.h"
#include "telemetry.h"

namespace eps {

/*!
 * Telemetry updater module. Updates telemetry values when ivoked.
 * @tparam telemetry Telemetry to update
 * @tparam InternalADC InternalADC from hal (hal::Analog::AnalogGPIO)
 * @tparam Eps EPS main structure
 */
template<eps::Telemetry& telemetry,
         template<hal::Analog::InternalADC::Input> typename InternalADC,
         typename Eps>
class TelemetryUpdater : hal::libs::PureStatic {
 public:
    /*!
     * Update "General" field in telemetry table.
     * Reads from physical devices and
     * stores result atomically in telemetry table.
     */
    static void update_general() {
        eps::Telemetry::General tm = telemetry.general;

        tm.controller_b.temperature        = Adc<AdcCh::ADC0>::read();
        tm.battery_controller.voltage      = Adc<AdcCh::ADC1>::read();
        tm.controller_a.supply_voltage     = Adc<AdcCh::ADC2>::read();
        tm.controller_b.supply_temperature = Adc<AdcCh::ADC3>::read();
        tm.battery_pack.temperature        = Adc<AdcCh::ADC5>::read();

        auto power_cycles              = eps::power_cycle_counters::get();
        tm.controller_b.safety_counter = power_cycles.safety;
        tm.controller_b.power_cycles   = power_cycles.all;

        static uint32_t uptime = 0;
        tm.controller_b.uptime = uptime++;

        telemetry.general = tm;
    }

 private:
    template<hal::Analog::InternalADC::Input input>
    using Adc = InternalADC<input>;

    using AdcCh = hal::Analog::InternalADC::Input;
};

}  // namespace eps

#endif  // EPS_B_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_
