#include <hal/hal>
#include <unity.h>

using namespace hal;

void TEST_ObcInterface();

int main() {
    Serial0.init(115200, hal::STDIO::ENABLE);

    TEST_ObcInterface();
}
