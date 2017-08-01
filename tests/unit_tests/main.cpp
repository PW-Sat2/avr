#include <avr/sleep.h>
#include <hal/hal>

using hal::Serial0;

void test_CommandDispatcher();
void test_Atomic();

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_CommandDispatcher();
    test_Atomic();

    hal::libs::sim::stop_simulation();
}
