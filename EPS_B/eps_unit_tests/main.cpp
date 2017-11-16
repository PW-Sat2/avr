#include <avr/sleep.h>
#include <hal/hal>

using hal::Serial0;

void test_TelemetryUpdater();
void test_MainTimer();
void test_Temperatures();
void test_CheckPins();

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_TelemetryUpdater();
    test_MainTimer();
    test_Temperatures();
    test_CheckPins();

    hal::libs::sim::stop_simulation();
}
