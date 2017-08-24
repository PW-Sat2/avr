#include <MainTimer.h>

#include "defs.h"


using namespace avr;

BatteryManager battery_manager;
eps::Telemetry Eps::telemetry;


void each_33ms();
void each_1sec();
void each_10sec();
void each_30min();

avr::Prescaler<33> timer_1second;
void each_33ms() {
    ControllerSpecialisation::each_33ms();

    Eps::LclCommander::handle_overcurrent();

    if (timer_1second.expired()) {
        each_1sec();
    }
}

avr::Prescaler<10> timer_10second;
void each_1sec() {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        ThermalKnives::tick();
        ObcWatchdog::tick();
    }
    Eps::TelemetryUpdater::update_general();

    // TODO(ggajoch): use real temperature
    OverheatProtection::tick(0);

    if (timer_10second.expired()) {
        each_10sec();
    }
}

avr::Prescaler<6 * 30> timer_30min;
void each_10sec() {
    // TODO(ggajoch): replace with real battery voltage when calibrated
    // TODO(ggajoch): replace with real temperatures when they are measured
    battery_manager.tick(10, 0);

    if (timer_30min.expired()) {
        each_30min();
    }
}

void each_30min() {
    LOG_INFO("Reset Safety Counter");
    eps::power_cycle_counters::reset();
}

void Eps::run() {
    while (1) {
        if (eps::MainTimer::expired()) {
            each_33ms();
        }
    }
}
