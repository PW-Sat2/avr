#include <hal/hal>
#include <queue>
#include "unity.h"

#include "commands/DisablePowerCycle.h"

bool armed   = false;
bool enabled = true;

eps::commands::ArmDisablePowerCycle<armed> arm_command;
eps::commands::DisablePowerCycle<armed, enabled> disable_command;

void test_DisablePowerCycle_arm() {
    armed = false;
    arm_command.invoke(std::array<uint8_t, 4>{0xDE, 0xAD, 0xBE, 0xEF});
    TEST_ASSERT_EQUAL(true, armed);
}

void test_DisablePowerCycle_arm_incorrect_length() {
    std::array<uint8_t, 6> dummy{0xDE, 0xAD, 0xBE, 0xEF, 0, 0};
    for (int i = 0; i < dummy.size(); ++i) {
        armed = true;
        arm_command.invoke(gsl::make_span(dummy.begin(), i));
        TEST_ASSERT_EQUAL(i == 4, armed);
    }
}

void test_DisablePowerCycle_arm_incorrect_data() {
    std::array<uint8_t, 4> dummy{0xDE, 0xAD, 0xBE, 0xEF};

    for (int i = 0; i < dummy.size(); ++i) {
        armed = true;

        dummy[i]++;

        arm_command.invoke(gsl::make_span(dummy.begin(), i));

        TEST_ASSERT_EQUAL(false, armed);

        dummy[i]--;
    }
}

void test_DisablePowerCycle_disable() {
    armed   = true;
    enabled = true;

    disable_command.invoke(
        std::array<uint8_t, 6>{0xDE, 0xFA, 0xCE, 0xD, 0xF0, 0x0D});

    TEST_ASSERT_EQUAL(true, armed);
    TEST_ASSERT_EQUAL(false, enabled);
}

void test_DisablePowerCycle_disable_not_armed() {
    armed   = false;
    enabled = true;

    disable_command.invoke(
        std::array<uint8_t, 6>{0xDE, 0xFA, 0xCE, 0xD, 0xF0, 0x0D});

    TEST_ASSERT_EQUAL(false, armed);
    TEST_ASSERT_EQUAL(true, enabled);
}

void test_DisablePowerCycle_disable_incorrect_length() {
    std::array<uint8_t, 8> dummy{0xDE, 0xFA, 0xCE, 0xD, 0xF0, 0x0D};

    for (int i = 0; i < dummy.size(); ++i) {
        armed   = true;
        enabled = false;

        disable_command.invoke(gsl::make_span(dummy.begin(), i));
        TEST_ASSERT_EQUAL(i != 6, enabled);
    }
}

void test_DisablePowerCycle_disable_incorrect_data() {
    armed = true;

    std::array<uint8_t, 6> dummy{0xDE, 0xFA, 0xCE, 0xD, 0xF0, 0x0D};

    for (int i = 0; i < dummy.size(); ++i) {
        enabled = false;

        dummy[i]++;

        disable_command.invoke(gsl::make_span(dummy.begin(), i));

        TEST_ASSERT_EQUAL(true, enabled);

        dummy[i]--;
    }
}

void test_DisablePowerCycle() {
    UnityBegin("");

    RUN_TEST(test_DisablePowerCycle_arm);
    RUN_TEST(test_DisablePowerCycle_disable_not_armed);
    RUN_TEST(test_DisablePowerCycle_arm_incorrect_length);
    RUN_TEST(test_DisablePowerCycle_arm_incorrect_data);

    RUN_TEST(test_DisablePowerCycle_disable);
    RUN_TEST(test_DisablePowerCycle_disable_incorrect_length);
    RUN_TEST(test_DisablePowerCycle_disable_incorrect_data);

    UnityEnd();
}
