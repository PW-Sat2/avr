#include <hal/hal>
#include <queue>
#include "unity.h"

#include "commands/DisablePowerCycle.h"

bool armed    = false;
bool disabled = false;

eps::commands::ArmDisablePowerCycle<armed> arm_command;
eps::commands::DisablePowerCycle<armed, disabled> disable_command;

void test_DisablePowerCycle_arm() {
    armed = false;
    arm_command.invoke(std::array<uint8_t, 4>{0xFE, 0xE1, 0xDE, 0xAD});
    TEST_ASSERT_EQUAL(true, armed);
}

void test_DisablePowerCycle_arm_incorrect_length() {
    std::array<uint8_t, 6> dummy{0xFE, 0xE1, 0xDE, 0xAD, 0, 0};
    for (int i = 0; i < dummy.size(); ++i) {
        armed = true;
        arm_command.invoke(gsl::make_span(dummy.begin(), i));
        TEST_ASSERT_EQUAL(i == 4, armed);
    }
}

void test_DisablePowerCycle_arm_incorrect_data() {
    std::array<uint8_t, 4> dummy{0xFE, 0xE1, 0xDE, 0xAD};

    for (int i = 0; i < dummy.size(); ++i) {
        armed = true;

        dummy[i]++;

        arm_command.invoke(gsl::make_span(dummy.begin(), i));

        TEST_ASSERT_EQUAL(false, armed);

        dummy[i]--;
    }
}

void test_DisablePowerCycle_disable() {
    armed    = true;
    disabled = false;

    disable_command.invoke(
        std::array<uint8_t, 6>{0xBA, 0xAD, 0xC0, 0x01, 0xC0, 0xDE});

    TEST_ASSERT_EQUAL(true, armed);
    TEST_ASSERT_EQUAL(true, disabled);
}

void test_DisablePowerCycle_disable_not_armed() {
    armed    = false;
    disabled = false;

    disable_command.invoke(
        std::array<uint8_t, 6>{0xBA, 0xAD, 0xC0, 0x01, 0xC0, 0xDE});

    TEST_ASSERT_EQUAL(false, armed);
    TEST_ASSERT_EQUAL(false, disabled);
}

void test_DisablePowerCycle_disable_incorrect_length() {
    std::array<uint8_t, 8> dummy{0xBA, 0xAD, 0xC0, 0x01, 0xC0, 0xDE};

    for (int i = 0; i < dummy.size(); ++i) {
        armed    = true;
        disabled = true;

        disable_command.invoke(gsl::make_span(dummy.begin(), i));
        TEST_ASSERT_EQUAL(i == 6, disabled);
    }
}

void test_DisablePowerCycle_disable_incorrect_data() {
    armed = true;

    std::array<uint8_t, 6> dummy{0xBA, 0xAD, 0xC0, 0x01, 0xC0, 0xDE};

    for (int i = 0; i < dummy.size(); ++i) {
        disabled = true;

        dummy[i]++;

        disable_command.invoke(gsl::make_span(dummy.begin(), i));

        TEST_ASSERT_EQUAL(false, disabled);

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
