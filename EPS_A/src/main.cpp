#include "eps.h"

int main() {
    avr::Eps::init();

    avr::Eps::run();

    hal::libs::sim::stop_simulation();
}
