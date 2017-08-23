#include <unity.h>
#include <hal/hal>

#include "commands/LCL.h"

struct LclCommanderMock {
    enum class Operation {
        None,
        On,
        Off,
        Error,
    };

    static Operation operation;
    static uint8_t last_id;

    static void setup() {
        operation = Operation::None;
        last_id   = 0;
    }
    static void check(LclCommanderMock::Operation operation_, uint8_t id) {
        TEST_ASSERT_EQUAL(operation_, operation);
        TEST_ASSERT_EQUAL(id, last_id);
    }

    static void mark(Operation oper, uint8_t id) {
        if (operation == Operation::None) {
            operation = oper;
            last_id   = id;
        } else {
            operation = Operation::Error;
        }
    }

    static void on(uint8_t id) {
        mark(Operation::On, id);
    }

    static void off(uint8_t id) {
        mark(Operation::Off, id);
    }
};
LclCommanderMock::Operation LclCommanderMock::operation;
uint8_t LclCommanderMock::last_id;

using cmd = LclCommanderMock;

eps::commands::EnableLCL<cmd> enable;
eps::commands::DisableLCL<cmd> disable;

void test_LCL_Commands_ON() {
    uint8_t i = 0;
    do {
        cmd::setup();
        enable.invoke(std::array<uint8_t, 1>{i});
        cmd::check(cmd::Operation::On, i);
    } while (i++ != 0xFF);
}

void test_LCL_Commands_OFF() {
    uint8_t i = 0;
    do {
        cmd::setup();
        disable.invoke(std::array<uint8_t, 1>{i});
        cmd::check(cmd::Operation::Off, i);
    } while (i++ != 0xFF);
}

void test_LCL_Commands() {
    UnityBegin("");

    RUN_TEST(test_LCL_Commands_ON);
    RUN_TEST(test_LCL_Commands_OFF);

    UnityEnd();
}
