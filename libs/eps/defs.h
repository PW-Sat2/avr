#ifndef LIBS_EPS_DEFS_H_
#define LIBS_EPS_DEFS_H_

#include "ObcWatchdog.h"
#include "OverheatProtection.h"
#include "PowerCycle.h"
#include "ThermalKnives.h"

#include "Prescaler.h"

#include "eps.h"

#include "settings.h"

namespace power_cycle {
extern bool enabled;
};

using FullPowerCycle = eps::FullPowerCycle<typename eps::IOMap::PowerCycle,
                                           typename eps::IOMap::Watchdog,
                                           power_cycle::enabled>;

using ObcWatchdog = eps::ObcWatchdog<FullPowerCycle::perform>;

using OverheatProtection = eps::OverheatProtection<avr::Eps::LclCommander, 80>;


using ThermalKnives =
    eps::ThermalKnives<typename eps::IOMap::ThermalKnives::Sail,
                       typename eps::IOMap::ThermalKnives::Sads>;


#endif  // LIBS_EPS_DEFS_H_
