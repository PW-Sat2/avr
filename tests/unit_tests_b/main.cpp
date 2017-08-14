#include <avr/sleep.h>
#include <hal/hal>

using hal::Serial0;

void test_BatteryManager();
void test_TwoStateFsm();

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_BatteryManager();
    test_TwoStateFsm();

    hal::libs::sim::stop_simulation();
}
