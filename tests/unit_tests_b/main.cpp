#include <avr/sleep.h>
#include <hal/hal>

using hal::Serial0;

void test_BatteryHeater();
void test_BatteryManager();
void test_TwoStateFsm();
void test_OverheatProtection();
void test_LCL_Commands();
void test_TK();
void test_ObcWatchdog();
void test_DisablePowerCycle();

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_BatteryHeater();
    test_BatteryManager();
    test_TwoStateFsm();
    test_OverheatProtection();
    test_LCL_Commands();
    test_TK();
    test_ObcWatchdog();
    test_DisablePowerCycle();

    hal::libs::sim::stop_simulation();
}
