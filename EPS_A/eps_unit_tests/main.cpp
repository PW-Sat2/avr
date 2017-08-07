#include <avr/sleep.h>
#include <hal/hal>

using hal::Serial0;

void test_TK();
void test_MainTimer();
void test_LCL_Commands();
void test_TelemetryUpdater();
void test_ObcWatchdog();

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_TK();
    test_MainTimer();
    test_LCL_Commands();
    test_TelemetryUpdater();
    test_ObcWatchdog();

    hal::libs::sim::stop_simulation();
}
