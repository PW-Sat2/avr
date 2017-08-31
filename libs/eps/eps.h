#ifndef LIBS_EPS_EPS_H_
#define LIBS_EPS_EPS_H_

#include "LclCommander.h"

#include "IOMap.h"
#include "TelemetryUpdater.h"
#include "telemetry.h"


namespace avr {

/*!
 * Main EPS structure.
 */
struct Eps {
    /*!
     * Used IOMap, derived from controller compile specification
     */
    using IOMap = eps::IOMap;

    /*!
     * LCL Commander - access to all LCLs
     */
    using LclCommander = eps::LclCommander<typename IOMap::AllLcls>;

    /*!
     * Telemetry object, sent to OBC
     */
    static eps::Telemetry telemetry;

    /*!
     * Telemetry Updater is invoked and telemetry is read periodically.
     */
    using TelemetryUpdater =
        eps::TelemetryUpdater<telemetry, hal::Analog::AnalogGPIO, Eps>;

    /*!
     * Initialises everything.
     * Invoke at the program start.
     */
    static void init();

    /*!
     * Continuous loop. Should never end.
     */
    static void run();

    static void overheat_submode_tick();
    static void battery_manager_tick();
};

}  // namespace avr

#endif  // LIBS_EPS_EPS_H_
