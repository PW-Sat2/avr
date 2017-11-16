#include <eps.h>
#include <unity.h>
#include "MainTimer.h"

using namespace eps;

void check(uint8_t ddr_val, uint8_t port_val, uint8_t pin) {
    auto output = hal::libs::read_bit(ddr_val, pin);
    auto pullup = hal::libs::read_bit(port_val, pin);

    TEST_ASSERT_TRUE(output || pullup);
}

void check_floating(uint8_t ddr_val, uint8_t port_val, uint8_t pin) {
    auto output = hal::libs::read_bit(ddr_val, pin);
    auto pullup = hal::libs::read_bit(port_val, pin);

    TEST_ASSERT_FALSE(output || pullup);
}

void check_pins() {
    avr::Eps::init();
    avr::Eps::TelemetryUpdater::update_general();

    for (uint8_t i = 0; i < 8; ++i) {
        check(DDRB, PORTB, i);
        check(DDRD, PORTD, i);
    }

    // ADC 0,1,2,3,5 used
    check_floating(DDRA, PORTA, 0);
    check_floating(DDRA, PORTA, 1);
    check_floating(DDRA, PORTA, 2);
    check_floating(DDRA, PORTA, 3);
    check(DDRA, PORTA, 4);
    check_floating(DDRA, PORTA, 5);
    check(DDRA, PORTA, 6);
    check(DDRA, PORTA, 7);

    // PORTC 2,3,4,5 - JTAG used
    check(DDRC, PORTC, 0);
    check(DDRC, PORTC, 1);
    check_floating(DDRC, PORTC, 2);
    check_floating(DDRC, PORTC, 3);
    check_floating(DDRC, PORTC, 4);
    check_floating(DDRC, PORTC, 5);
    check(DDRC, PORTC, 6);
    check(DDRC, PORTC, 7);
}

void test_CheckPins() {
    UnityBegin("");

    RUN_TEST(check_pins);

    UnityEnd();
}
