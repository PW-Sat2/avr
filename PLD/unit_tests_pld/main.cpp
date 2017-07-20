#include <hal/hal>

using hal::Serial0;

void test_commands();
void test_AD7714();

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    test_commands();
    test_AD7714();

    hal::libs::sim::stop_simulation();
}
