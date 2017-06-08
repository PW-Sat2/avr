extern "C" {
#include "unity_fixture.h"
}

#include "TelemetryBuffer/TelemetryBuffer.h"

TEST_GROUP(TelemetryBuffer);
TEST_GROUP_RUNNER(TelemetryBuffer) {
    RUN_TEST_CASE(TelemetryBuffer, actualInt);
    RUN_TEST_CASE(TelemetryBuffer, actualStruct);
    RUN_TEST_CASE(TelemetryBuffer, buffer);
    RUN_TEST_CASE(TelemetryBuffer, callback);
}


TEST_SETUP(TelemetryBuffer) {
}

TEST_TEAR_DOWN(TelemetryBuffer) {
}

TEST(TelemetryBuffer, actualInt) {
    TelemetryBuffer<int> buffer([](auto) {});

    *buffer.actual = 10;
    TEST_ASSERT_EQUAL_INT(10, *buffer.actual);

    *buffer.actual = 0x1FAA;
    TEST_ASSERT_EQUAL_INT(0x1FAA, *buffer.actual);
}

struct XXX {
    int a;
    float b;
    bool operator==(const XXX& rhs) const {
        return a == rhs.a && b == rhs.b;
    }
};

TEST(TelemetryBuffer, actualStruct) {
    TelemetryBuffer<XXX> buffer([](auto) {});

    XXX now = {.a = 1, .b = 2.56};

    *buffer.actual = now;
    TEST_ASSERT_TRUE(now == *buffer.actual);
}

TEST(TelemetryBuffer, buffer) {
    TelemetryBuffer<int> buffer([](auto) {});

    *buffer.actual   = 10;
    *buffer.buffered = 20;
    TEST_ASSERT_EQUAL_INT(10, *buffer.actual);

    buffer.update();
    TEST_ASSERT_EQUAL_INT(20, *buffer.actual);
    TEST_ASSERT_EQUAL_INT(20, *buffer.buffered);

    *buffer.buffered = 30;
    TEST_ASSERT_EQUAL_INT(20, *buffer.actual);
    TEST_ASSERT_EQUAL_INT(30, *buffer.buffered);
    buffer.update();
    TEST_ASSERT_EQUAL_INT(30, *buffer.actual);
    TEST_ASSERT_EQUAL_INT(30, *buffer.buffered);
}


int* received_ptr;
void callback(gsl::not_null<int*> ptr) {
    received_ptr = ptr;
}
TEST(TelemetryBuffer, callback) {
    TelemetryBuffer<int> buffer(callback);

    auto _actual   = buffer.actual;
    auto _buffered = buffer.buffered;

    *buffer.actual   = 10;
    *buffer.buffered = 20;

    received_ptr = nullptr;
    buffer.update();
    TEST_ASSERT_EQUAL_INT(buffer.actual, received_ptr);
    TEST_ASSERT_EQUAL_INT(_buffered, received_ptr);
    TEST_ASSERT_EQUAL_INT(20, *received_ptr);

    received_ptr = nullptr;
    buffer.update();
    TEST_ASSERT_EQUAL_INT(buffer.actual, received_ptr);
    TEST_ASSERT_EQUAL_INT(_actual, received_ptr);
    TEST_ASSERT_EQUAL_INT(20, *received_ptr);

    *buffer.buffered = 30;
    received_ptr     = nullptr;
    buffer.update();
    TEST_ASSERT_EQUAL_INT(buffer.actual, received_ptr);
    TEST_ASSERT_EQUAL_INT(_buffered, received_ptr);
    TEST_ASSERT_EQUAL_INT(30, *received_ptr);
}
