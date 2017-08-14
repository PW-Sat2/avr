#include "MainTimer.h"
#include "Prescaler.h"

#include "eps/eps.h"
#include "eps/main.h"

void each_33ms();
void each_1sec();


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

void each_1sec() {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        ThermalKnives::tick();
        ObcWatchdog::tick();
    }
    TelemetryUpdater::update_general();
}

int main() {
    eps_a::init_hardware();
    LOG_INFO("Loaded EPS_A software rev. " GIT_REVISION
             ", local state: " GIT_CHANGES);
    avr::power_cycle_counters::bootup(eps_a::emergency_sail_deployment);
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
