#include <unity.h>
#include <cstdint>
#include <gsl/gsl>

#include "ObcInterface/CommandDispatcher.h"

class CommandMock {
 public:
    CommandMock() {
        invoked_ = false;
    }
    bool invoked() {
        auto _   = invoked_;
        invoked_ = false;
        return _;
    }

    bool invoked_;
    gsl::span<const uint8_t> params_buffer;
};

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        cmd.invoke(args);
    }
} executor;

template<uint8_t opcode, uint8_t parameters>
class MockCommand : public Command<opcode, parameters> {
 public:
    MockCommand(CommandMock& mock) : mock(mock) {
    }

    void invoke(gsl::span<const uint8_t> params) {
        LOG_INFO("RadFET at %p", this);
        TEST_ASSERT_EQUAL_INT(parameters, params.size());
        mock.invoked_      = true;
        mock.params_buffer = params;
    }
    CommandMock& mock;
};

using RadFETCommand = MockCommand<0xAB, 0>;
using SunSCommand   = MockCommand<0xDC, 3>;


CommandMock radFETCommand;
CommandMock sunSCommand;

CommandDispatcher<Executor, RadFETCommand, SunSCommand>
    cmd(executor, RadFETCommand(radFETCommand), SunSCommand(sunSCommand));

void test_CommandDispatcher_noCommandInvoked() {
    uint8_t data[] = {0x00};
    cmd.parse(data);
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

void test_CommandDispatcher_tooShortCommand() {
    uint8_t data[] = {};
    cmd.parse(gsl::make_span(data, 0));
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

void test_CommandDispatcher_radfetNoParams() {
    uint8_t data[] = {0xAB};
    cmd.parse(data);
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_TRUE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());

    cmd.parse(data);
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_TRUE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

void test_CommandDispatcher_radfetWrongNumberOfParams() {
    uint8_t data[] = {0xAB, 1};
    cmd.parse(data);
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

void test_CommandDispatcher_sunsParamsOK() {
    uint8_t data[] = {0xDC, 1, 2, 3};
    cmd.parse(data);
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_TRUE(sunSCommand.invoked());
    uint8_t params[] = {1, 2, 3};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(params, sunSCommand.params_buffer.data(), 3);
}

void test_CommandDispatcher_commandGetsDispatched() {
    uint8_t data[] = {0xDC, 0xFF, 0xFF, 0xFF};
    cmd.parse(data);
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_TRUE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

void test_CommandDispatcher_bufferCorruptedAfterParse() {
    uint8_t data[] = {0xDC, 1, 2, 3};
    cmd.parse(data);
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());

    data[0] = 0;
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_TRUE(sunSCommand.invoked());
    uint8_t params[] = {1, 2, 3};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(params, sunSCommand.params_buffer.data(), 3);
}

void test_CommandDispatcher_tooLongCommandDoesNotHarm() {
    uint8_t data[] = {0xDC};
    cmd.parse(data);
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

void test_CommandDispatcher_dataRequestFilter() {
    uint8_t data[] = {0xDC};
    cmd.parse(data);
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

void test_CommandDispatcher() {
    UnityBegin("");

    RUN_TEST(test_CommandDispatcher_noCommandInvoked);
    RUN_TEST(test_CommandDispatcher_tooShortCommand);
    RUN_TEST(test_CommandDispatcher_radfetNoParams);
    RUN_TEST(test_CommandDispatcher_radfetWrongNumberOfParams);
    RUN_TEST(test_CommandDispatcher_sunsParamsOK);
    RUN_TEST(test_CommandDispatcher_commandGetsDispatched);
    RUN_TEST(test_CommandDispatcher_bufferCorruptedAfterParse);
    RUN_TEST(test_CommandDispatcher_tooLongCommandDoesNotHarm);
    RUN_TEST(test_CommandDispatcher_dataRequestFilter);

    UnityEnd();
}
