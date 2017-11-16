#include <avr/sleep.h>
#include <hal/hal>

using hal::Serial0;

void test_MainTimer();
void test_TelemetryUpdater();
void test_Temperatures();
void test_MpptUpdater();
void test_CheckPins();

int main() {
    Serial0.init(1000000);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_MainTimer();
    test_TelemetryUpdater();
    test_Temperatures();
    test_MpptUpdater();
    test_CheckPins();

    hal::libs::sim::stop_simulation();
}
