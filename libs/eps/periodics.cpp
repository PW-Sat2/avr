#include <MainTimer.h>

#include "defs.h"


using namespace avr;

Eps::BatteryManager Eps::battery_manager;
eps::Telemetry Eps::telemetry;


void each_33ms();
void each_1sec();
void each_10sec();
void each_30min();

avr::Prescaler<30> timer_1second;
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

    OverheatProtection::tick(ControllerSpecialisation::max_eps_temperature());

    if (timer_10second.expired()) {
        each_10sec();
    }
}

avr::Prescaler<6 * 30> timer_30min;
void each_10sec() {
    Eps::battery_manager.tick(ControllerSpecialisation::battery_voltage(),
                              ControllerSpecialisation::battery_temperature());

    if (timer_30min.expired()) {
        each_30min();
    }
}

void each_30min() {
    LOG_INFO("Reset Safety Counter");
    eps::power_cycle_counters::reset();
}

void Eps::run() {
    Eps::IOMap::SerialRx::init(hal::DigitalIO::Mode::OUTPUT);

    while (1) {
        if (eps::MainTimer::expired()) {
            Eps::IOMap::SerialRx::set();
            each_33ms();
            Eps::IOMap::SerialRx::reset();
        } else {
            sleep_mode();
        }
    }
}
