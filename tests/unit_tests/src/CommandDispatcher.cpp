#include "ObcInterface/CommandDispatcher.h"
#include <cstdint>
#include <gsl/gsl>
extern "C" {
#include "unity_fixture.h"
};

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
    void invoke(gsl::span<uint8_t> params) override {
        TEST_ASSERT_EQUAL_INT(0, params.size());
        invoked_ = true;
    }
};

class SunSCommand : public CommandWithOpcode<0xDC, 3>, public CommandMock {
 public:
    gsl::span<uint8_t> params_buffer;

 private:
    void invoke(gsl::span<uint8_t> params) override {
        TEST_ASSERT_EQUAL_INT(3, params.size());
        invoked_      = true;
        params_buffer = params;
    }
};

TEST_GROUP(CommandDispatcher);

RadFETCommand radFETCommand;
SunSCommand sunSCommand;

CommandDispatcher<RadFETCommand, SunSCommand> cmd;


template<class... T>
void foo(T... t) {
}
TEST_SETUP(CommandDispatcher) {
    foo(radFETCommand, sunSCommand);
    cmd = CommandDispatcher<RadFETCommand, SunSCommand>(
        {&radFETCommand, &sunSCommand});
}
TEST_TEAR_DOWN(CommandDispatcher) {
    radFETCommand.invoked();
    sunSCommand.invoked();
}

TEST_GROUP_RUNNER(commands) {
    //    RUN_TEST_CASE(CommandDispatcher, noCommandsDoesNotHarm); printf(" ");
    RUN_TEST_CASE(CommandDispatcher, noCommandInvoked);
    printf(" ");
    RUN_TEST_CASE(CommandDispatcher, tooShortCommand);
    printf(" ");
    RUN_TEST_CASE(CommandDispatcher, radfetNoParams);
    printf(" ");
    RUN_TEST_CASE(CommandDispatcher, radfetWrongNumberOfParams);
    printf(" ");
    RUN_TEST_CASE(CommandDispatcher, sunsParamsOK);
    printf(" ");
    RUN_TEST_CASE(CommandDispatcher, commandGetsDispatched);
    printf(" ");
    RUN_TEST_CASE(CommandDispatcher, bufferCorruptedAfterParse);
    printf(" ");
    RUN_TEST_CASE(CommandDispatcher, tooLongCommandDoesNotHarm);
    printf(" ");
    RUN_TEST_CASE(CommandDispatcher, dataRequestFilter);
    printf(" ");
}

// TEST(CommandDispatcher, noCommandsDoesNotHarm) {
//    cmd = CommandDispatcher({});
//    uint8_t data[] = {0x00};
//    cmd.parse(data);
//    TEST_ASSERT_FALSE(radFETCommand.invoked());
//    TEST_ASSERT_FALSE(sunSCommand.invoked());
//    cmd.dispatch();
//    TEST_ASSERT_FALSE(radFETCommand.invoked());
//    TEST_ASSERT_FALSE(sunSCommand.invoked());
//}

TEST(CommandDispatcher, noCommandInvoked) {
    uint8_t data[] = {0x00};
    cmd.parse(data);
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

TEST(CommandDispatcher, tooShortCommand) {
    uint8_t data[] = {};
    cmd.parse(gsl::make_span(data, 0));
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

TEST(CommandDispatcher, radfetNoParams) {
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

TEST(CommandDispatcher, radfetWrongNumberOfParams) {
    uint8_t data[] = {0xAB, 1};
    cmd.parse(data);
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

TEST(CommandDispatcher, sunsParamsOK) {
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

TEST(CommandDispatcher, commandGetsDispatched) {
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

TEST(CommandDispatcher, bufferCorruptedAfterParse) {
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

TEST(CommandDispatcher, tooLongCommandDoesNotHarm) {
    uint8_t data[100] = {0xDC};
    cmd.parse(data);
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}

TEST(CommandDispatcher, dataRequestFilter) {
    uint8_t data[100] = {0xDC};
    cmd.parse(data);
    cmd.dispatch();
    TEST_ASSERT_FALSE(radFETCommand.invoked());
    TEST_ASSERT_FALSE(sunSCommand.invoked());
}
