#include "eps.h"

#include "IOMap.h"


using namespace hal;


struct ThisControllerInit {
    static void init() {
        eps_a::iomap::mux::Mux::init();
    }
};

using Eps =
    Epss<'A', 0x35, 0x61, eps_a::Telemetry, eps_a::IOMap, ThisControllerInit>;


ISR(TWI_vect) {
    Eps::TWI_ISR();
}


int main() {
    Eps::init();

    Eps::run();

    hal::libs::sim::stop_simulation();
}
