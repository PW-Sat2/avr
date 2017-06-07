#include <unity.h>
#include <hal/hal>

using namespace hal;

void TEST_ObcInterface();

int main() {
    Serial0.init(115200);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    TEST_ObcInterface();
}
