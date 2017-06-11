#include "ObcInterface/CommandDispatcher.h"
#include <cstdint>
#include <gsl/gsl>
#include "unity.h"

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

 protected:
    bool invoked_;
};

class RadFETCommand : public CommandWithOpcode<0xAB, 0>, public CommandMock {
 public:
 private:
    void invoke(gsl::span<const uint8_t> params) override {
        TEST_ASSERT_EQUAL_INT(0, params.size());
        invoked_ = true;
    }
};

class SunSCommand : public CommandWithOpcode<0xDC, 3>, public CommandMock {
 public:
    gsl::span<const uint8_t> params_buffer;

 private:
    void invoke(gsl::span<const uint8_t> params) override {
        TEST_ASSERT_EQUAL_INT(3, params.size());
        invoked_      = true;
        params_buffer = params;
    }
};

RadFETCommand radFETCommand;
SunSCommand sunSCommand;

CommandDispatcher<RadFETCommand, SunSCommand> cmd;

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
    cmd = CommandDispatcher<RadFETCommand, SunSCommand>(
        {&radFETCommand, &sunSCommand});

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
