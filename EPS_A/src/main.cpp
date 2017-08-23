#include "MainTimer.h"
#include "Prescaler.h"

#include "eps/eps.h"
#include "eps/main.h"

#include "BatteryManager.h"
#include "PowerCycleCounter.h"

void each_33ms();
void each_1sec();
void each_10sec();
void each_30min();


eps_a::Telemetry telemetry;
EPSACommandDispatcher dispatcher;

ISR(TWI_vect) {
    Obc::process_interrupt();
    dispatcher.dispatch();
}
void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

avr::Prescaler<33> timer_1second;
void each_33ms() {
    TelemetryUpdater::update_mppt();
    LclCommander::handle_overcurrent();

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
    TelemetryUpdater::update_general();

    // TODO(ggajoch): use real temperature
    OverheatProtection::tick(0);

    if (timer_10second.expired()) {
        each_10sec();
    }
}

eps::BatteryManager<eps_a::iomap::battery_controller::PinCharge,
                    eps_a::iomap::battery_controller::PinDischarge,
                    eps_a::iomap::battery_controller::PinHeater>
    battery_manager;

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


int main() {
    eps_a::init_hardware();
    LOG_INFO("Loaded EPS_A software rev. " GIT_REVISION
             ", local state: " GIT_CHANGES);
    eps::power_cycle_counters::bootup(eps_a::emergency_sail_deployment);
    eps_a::init_obc_interface();

    sei();

    LOG_INFO("EPS A initialized.");

    while (1) {
        if (eps_a::MainTimer::expired()) {
            each_33ms();
        }
    }

    hal::libs::sim::stop_simulation();
}
