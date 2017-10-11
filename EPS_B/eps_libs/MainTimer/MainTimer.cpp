#include "MainTimer.h"

using namespace eps;

static volatile bool timer2_expired = false;

ISR(TIMER2_COMPA_vect) {
    timer2_expired = true;
}

void MainTimer::init() {
    TCCR2A = (1 << WGM21) | (0 << WGM20);  // CTC mode
    TCCR2B = (0 << WGM22) | (1 << CS22) | (1 << CS21) |
             (0 << CS20);  // prescaler 256

    // output compare = 130
    // result frequency = 30.04 Hz
    OCR2A  = 130;
    TCNT2  = 0;
    TIMSK2 = (1 << OCIE2A);  // compare A compare
}

bool MainTimer::expired() {
    if (timer2_expired) {
        timer2_expired = false;

        return true;
    }
    return false;
}
