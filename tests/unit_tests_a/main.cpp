#include <avr/sleep.h>
#include <hal/hal>

using hal::Serial0;

void test_CommandDispatcher();
void test_Atomic();
void test_SoftwareTimer();
void test_Prescaler();
void test_LclInterface();
void test_LclCommander();
void test_RedundancyTest();
void test_PowerCycleCounter();
void test_calibration();

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_CommandDispatcher();
    test_Atomic();
    test_SoftwareTimer();
    test_Prescaler();
    test_LclInterface();
    test_LclCommander();
    test_RedundancyTest();
    test_PowerCycleCounter();
    test_calibration();

    hal::libs::sim::stop_simulation();
}
