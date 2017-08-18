#include "eps.h"

#include "main.h"

using namespace hal;

void eps_b::init_hardware() {
    hal::Watchdog::disable();

    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();
}

void eps_b::init_obc_interface() {
    telemetry.init();
    telemetry.who_am_i = 0x9D;
    Obc::init(&telemetry);
}
