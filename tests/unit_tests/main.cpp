#include <avr/sleep.h>
#include <hal/hal>
#include "unity.h"

using hal::Serial0;

void test_CommandDispatcher();

int main() {
    Serial0.init(115200);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_CommandDispatcher();

    cli();
    sleep_mode();
}
