#include "MainTimer.h"

using namespace eps;

static volatile bool timer0_expired = false;

ISR(TIMER0_COMP_vect) {
    timer0_expired = true;
}

void MainTimer::init() {
    hal::Timer0::init(hal::Timer0::Prescaler::DIV_1024, hal::Timer0::Mode::CTC);

    OCR0 = 252 - 1;
    hal::Timer0::enable_compare_interrupt();
    timer0_expired = false;

    TCNT0 = 0;
    // overflow 31 Hz
}

bool MainTimer::expired() {
    if (timer0_expired) {
        timer0_expired = false;

        return true;
    }
    return false;
}
