#ifndef LIBS_EPS_DEFS_H_
#define LIBS_EPS_DEFS_H_

#include <BatteryManager.h>


#include "ObcWatchdog.h"
#include "OverheatProtection.h"
#include "PowerCycle.h"
#include "ThermalKnives.h"

#include "Prescaler.h"

#include "eps.h"

#include "settings.h"

using FullPowerCycle =
    eps::FullPowerCycle<typename eps::IOMap::PowerCycle, typename eps::IOMap::Watchdog>;

using ObcWatchdog = eps::ObcWatchdog<FullPowerCycle::perform>;

using OverheatProtection = eps::OverheatProtection<avr::Eps::LclCommander, 50>;


using ThermalKnives =
    eps::ThermalKnives<typename eps::IOMap::ThermalKnives::Sail,
                       typename eps::IOMap::ThermalKnives::Sads>;


using BatteryManager = eps::BatteryManager<typename eps::IOMap::Battery::Charge,
                                           typename eps::IOMap::Battery::Discharge,
                                           typename eps::IOMap::Battery::Heater>;


#endif  // LIBS_EPS_DEFS_H_
