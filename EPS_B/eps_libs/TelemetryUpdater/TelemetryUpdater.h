#ifndef EPS_B_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_
#define EPS_B_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_

#include <hal/hal>
#include "PowerCycleCounter.h"
#include "telemetry.h"

namespace eps {

/*!
 * Telemetry updater module. Updates telemetry values when ivoked.
 * This is split in two parts: general (1 second interval) and MPPT (33 ms)
 * @tparam telemetry Telemetry to update
 * @tparam Mux ADG709 mux type (device from IOMap)
 * @tparam InternalADC InternalADC from hal (hal::Analog::AnalogGPIO)
 * @tparam Mppt MPPT structure to use (from IOMap)
 * @tparam LclCommander LCL commander to use to get statuses of LCLs
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
        //        eps_b::Telemetry::General tm = telemetry.general;
    }

 private:
    using MuxCh = hal::devices::ADG709::Channel;

    template<hal::Analog::InternalADC::Input input>
    using Adc = InternalADC<input>;

    using AdcCh = hal::Analog::InternalADC::Input;
};

}  // namespace eps

#endif  // EPS_B_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_
