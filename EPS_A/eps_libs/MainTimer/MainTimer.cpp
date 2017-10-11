#include "MainTimer.h"

using namespace eps;

static volatile bool timer0_expired = false;

ISR(TIMER0_OVF_vect) {
    timer0_expired = true;
}

void MainTimer::init() {
    hal::Timer0::init(hal::Timer0::Prescaler::DIV_1024, hal::Timer0::Mode::Normal);

    TCNT0 = 0;
    hal::Timer0::enable_overflow_interrupt();
    // overflow each 30.5 ms
}

bool MainTimer::expired() {
    if (timer0_expired) {
        timer0_expired = false;

        return true;
    }
    return false;
}
