#include <eps.h>
#include <settings.h>
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
    for (int i = 0; i < 3; ++i) {
        // three phases of MPPT
        ControllerSpecialisation::each_33ms();
    }

    for (uint8_t i = 0; i < 8; ++i) {
        check(DDRA, PORTA, i);
        check(DDRB, PORTB, i);
        check(DDRC, PORTC, i);
        check(DDRD, PORTD, i);
        check(DDRE, PORTE, i);

        // PF0-PF3 - ADC0-3
        // PF4-PF7 - JTAG
        check_floating(DDRF, PORTF, i);
    }

    check(DDRG, PORTG, 0);
    check(DDRG, PORTG, 1);
    check(DDRG, PORTG, 2);
    check(DDRG, PORTG, 3);
    check(DDRG, PORTG, 4);
}

void test_CheckPins() {
    UnityBegin("");

    RUN_TEST(check_pins);

    UnityEnd();
}
