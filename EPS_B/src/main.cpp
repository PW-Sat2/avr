#include <telemetry.h>
#include <hal/hal>

#include "logger.h"

#include "eps/eps.h"

eps_b::Telemetry telemetry;

int main() {
    eps_b::init_hardware();
    eps_b::init_obc_interface();

    sei();

    LOG_INFO("EPS A initialized.");

    hal::libs::sim::stop_simulation();
}
