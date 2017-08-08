#include "MainTimer.h"
#include "Prescaler.h"

#include "eps/eps.h"
#include "eps/main.h"

eps_a::Telemetry telemetry;
EPSACommandDispatcher dispatcher;

ISR(TWI_vect) {
    Obc::process_interrupt();
    dispatcher.dispatch();
}

int main() {
    eps_a::init_hardware();
    LOG_INFO("Loaded EPS_A software rev. " GIT_REVISION
             ", local state: " GIT_CHANGES);
    eps_a::init_obc_interface();

    sei();

    avr::Prescaler<33> timer_1second;

    LOG_INFO("EPS A initialized.");

    while (1) {
        if (eps_a::MainTimer::expired()) {
            TelemetryUpdater::update_mppt();
            if (timer_1second.expired()) {
                ThermalKnives::tick();
                TelemetryUpdater::update_general();
            }
        }
    }

    hal::libs::sim::stop_simulation();
}
