#include <unity.h>
#include <hal/hal>

#include "OverheatProtection.h"

bool lcls_on = true;
struct LclCommanderMock {
    static void off_all() {
        lcls_on = false;
    }
};

static void reset() {
    lcls_on = true;
}
static void check_lcls(bool status) {
    TEST_ASSERT_EQUAL(status, lcls_on);
    reset();
}

using overheat = eps::OverheatProtection<LclCommanderMock, 20>;

void test_OverheatProtection_enabled_by_default() {
    reset();

    for (float temp = -50; temp <= 20; temp += 1) {
        overheat::tick(temp);
        check_lcls(true);
    }

    overheat::tick(20.1);
    check_lcls(false);

    reset();
    overheat::tick(50);
    check_lcls(false);

    reset();
    overheat::tick(20);
    check_lcls(true);
}

void test_OverheatProtection_disabled() {
    reset();
    overheat::disable();

    for (float temp = -50; temp <= 100; temp += 1) {
        overheat::tick(temp);
        check_lcls(true);
    }
}

void test_OverheatProtection() {
    UnityBegin("");

    RUN_TEST(test_OverheatProtection_enabled_by_default);
    RUN_TEST(test_OverheatProtection_disabled);

    UnityEnd();
}
