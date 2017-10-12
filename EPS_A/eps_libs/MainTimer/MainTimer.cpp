#include "MainTimer.h"

using namespace eps;

static volatile bool timer0_expired = false;

ISR(TIMER0_COMP_vect) {
    timer0_expired = true;
}

void MainTimer::init() {
    TCCR0 = 0b1110;  // CTC, prescaler 256

    OCR0  = 126 - 1;
    TIMSK = (1 << OCIE0);  // Output Compare 0 interrupt only

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
