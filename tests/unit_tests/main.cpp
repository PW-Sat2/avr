#include <hal/hal>
extern "C" {
#include "unity_fixture.h"
};

using hal::Serial0;

static void run_all_tests() {
    RUN_TEST_GROUP(commands);
    RUN_TEST_GROUP(TelemetryBuffer);
}

int main() {
    Serial0.init(115200);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    const char* no_args[] = {""};
    UnityMain(0, no_args, run_all_tests);
}
