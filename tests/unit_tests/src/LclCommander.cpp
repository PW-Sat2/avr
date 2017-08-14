#include <unity.h>
#include <hal/hal>

#include "LclCommander.h"

using namespace eps;

template<int id_, uint8_t bit_pos_>
struct LCL {
    enum class Id : std::uint8_t {};

    using IdType               = Id;
    static constexpr IdType id = static_cast<Id>(id_);

    static constexpr uint8_t bit_pos = bit_pos_;

    static constexpr char name[] = "";

    static bool initialised;
    static bool state_on;

    static void init() {
        initialised = true;
    }

    static void on() {
        state_on = true;
    }

    static void off() {
        state_on = false;
    }

    static bool is_on_invoked;
    static void check_is_on_invocation() {
        TEST_ASSERT_TRUE(is_on_invoked);
        is_on_invoked = false;
    }
    static bool is_on() {
        is_on_invoked = true;
        return state_on;
    }

    static bool overcurrent_;
    static bool overcurrent() {
        return overcurrent_;
    }
};
template<int id_, uint8_t bit_pos_>
bool LCL<id_, bit_pos_>::state_on;
template<int id_, uint8_t bit_pos_>
bool LCL<id_, bit_pos_>::initialised;
template<int id_, uint8_t bit_pos_>
bool LCL<id_, bit_pos_>::overcurrent_;
template<int id_, uint8_t bit_pos_>
constexpr char LCL<id_, bit_pos_>::name[];
template<int id_, uint8_t bit_pos_>
bool LCL<id_, bit_pos_>::is_on_invoked;

using LCLs = std::tuple<LCL<0, 0>, LCL<20, 1>, LCL<7, 2>, LCL<0xFB, 3>>;
using cmd  = LclCommander<LCLs>;

void set_on(bool a, bool b, bool c, bool d) {
    std::tuple_element_t<0, LCLs>::state_on = a;
    std::tuple_element_t<1, LCLs>::state_on = b;
    std::tuple_element_t<2, LCLs>::state_on = c;
    std::tuple_element_t<3, LCLs>::state_on = d;
}

void test_on(bool a, bool b, bool c, bool d) {
    TEST_ASSERT_EQUAL(a, (std::tuple_element_t<0, LCLs>::state_on));
    TEST_ASSERT_EQUAL(b, (std::tuple_element_t<1, LCLs>::state_on));
    TEST_ASSERT_EQUAL(c, (std::tuple_element_t<2, LCLs>::state_on));
    TEST_ASSERT_EQUAL(d, (std::tuple_element_t<3, LCLs>::state_on));

    uint8_t val = (hal::libs::ToInt(d) << 3) | (hal::libs::ToInt(c) << 2) |
                  (hal::libs::ToInt(b) << 1) | (hal::libs::ToInt(a) << 0);
    TEST_ASSERT_EQUAL(val, cmd::on_status());
}

void reset_initialised() {
    std::tuple_element_t<0, LCLs>::initialised = false;
    std::tuple_element_t<1, LCLs>::initialised = false;
    std::tuple_element_t<2, LCLs>::initialised = false;
    std::tuple_element_t<3, LCLs>::initialised = false;
}

void test_initialised() {
    TEST_ASSERT_TRUE((std::tuple_element_t<0, LCLs>::initialised));
    TEST_ASSERT_TRUE((std::tuple_element_t<1, LCLs>::initialised));
    TEST_ASSERT_TRUE((std::tuple_element_t<2, LCLs>::initialised));
    TEST_ASSERT_TRUE((std::tuple_element_t<3, LCLs>::initialised));
}

void set_overcurrent(bool a, bool b, bool c, bool d) {
    std::tuple_element_t<0, LCLs>::overcurrent_ = a;
    std::tuple_element_t<1, LCLs>::overcurrent_ = b;
    std::tuple_element_t<2, LCLs>::overcurrent_ = c;
    std::tuple_element_t<3, LCLs>::overcurrent_ = d;
}

void test_LclCommander_notfound() {
    set_on(true, true, true, true);

    cmd::on(1);
    cmd::off(1);

    test_on(true, true, true, true);

    set_on(false, false, false, false);

    cmd::on(1);
    cmd::off(1);

    test_on(false, false, false, false);
}

void test_LclCommander_init() {
    reset_initialised();

    cmd::init();

    test_initialised();
}

void test_LclCommander_on() {
    set_on(0, 0, 0, 0);

    cmd::on(0);
    test_on(1, 0, 0, 0);

    cmd::on(7);
    test_on(1, 0, 1, 0);

    cmd::on(0xFB);
    test_on(1, 0, 1, 1);

    cmd::on(0xFA);
    test_on(1, 0, 1, 1);

    cmd::on(20);
    test_on(1, 1, 1, 1);
}

void test_LclCommander_off() {
    set_on(1, 1, 1, 1);

    cmd::off(0);
    test_on(0, 1, 1, 1);

    cmd::off(7);
    test_on(0, 1, 0, 1);

    cmd::off(0xFB);
    test_on(0, 1, 0, 0);

    cmd::off(0xFA);
    test_on(0, 1, 0, 0);

    cmd::off(20);
    test_on(0, 0, 0, 0);
}

void test_LclCommander_overcurrent() {
    set_on(1, 1, 1, 1);

    set_overcurrent(0, 0, 0, 0);
    cmd::handle_overcurrent();
    test_on(1, 1, 1, 1);
    TEST_ASSERT_EQUAL(0b1111, cmd::overcurrent_status());

    set_overcurrent(0, 0, 0, 1);
    cmd::handle_overcurrent();
    test_on(1, 1, 1, 0);
    TEST_ASSERT_EQUAL(0b0111, cmd::overcurrent_status());
    cmd::handle_overcurrent();
    test_on(1, 1, 1, 0);
    TEST_ASSERT_EQUAL(0b0111, cmd::overcurrent_status());
    cmd::handle_overcurrent();
    test_on(1, 1, 1, 0);
    TEST_ASSERT_EQUAL(0b0111, cmd::overcurrent_status());

    set_overcurrent(0, 0, 1, 1);
    cmd::handle_overcurrent();
    test_on(1, 1, 0, 0);
    TEST_ASSERT_EQUAL(0b0011, cmd::overcurrent_status());

    set_overcurrent(0, 0, 0, 0);
    cmd::handle_overcurrent();
    test_on(1, 1, 0, 0);
    TEST_ASSERT_EQUAL(0b0011, cmd::overcurrent_status());

    set_overcurrent(1, 0, 0, 0);
    cmd::handle_overcurrent();
    test_on(0, 1, 0, 0);
    TEST_ASSERT_EQUAL(0b0010, cmd::overcurrent_status());

    set_on(1, 1, 1, 1);
    set_overcurrent(1, 0, 1, 1);
    cmd::handle_overcurrent();
    test_on(0, 1, 0, 0);
    TEST_ASSERT_EQUAL(0b0010, cmd::overcurrent_status());

    set_overcurrent(0, 1, 0, 0);
    cmd::handle_overcurrent();
    test_on(0, 0, 0, 0);
    TEST_ASSERT_EQUAL(0b0000, cmd::overcurrent_status());
}

void test_LclCommander_overcurrent_reset_by_on_off() {
    set_overcurrent(1, 1, 1, 1);
    cmd::handle_overcurrent();
    test_on(0, 0, 0, 0);
    TEST_ASSERT_EQUAL(0b0000, cmd::overcurrent_status());

    set_overcurrent(0, 0, 1, 0);

    cmd::on(0);
    TEST_ASSERT_EQUAL(0b0001, cmd::overcurrent_status());

    cmd::on(0);
    TEST_ASSERT_EQUAL(0b0001, cmd::overcurrent_status());

    cmd::on(7);
    TEST_ASSERT_EQUAL(0b0101, cmd::overcurrent_status());

    cmd::on(0xFB);
    TEST_ASSERT_EQUAL(0b1101, cmd::overcurrent_status());

    cmd::on(20);
    TEST_ASSERT_EQUAL(0b1111, cmd::overcurrent_status());
    cmd::handle_overcurrent();
    TEST_ASSERT_EQUAL(0b1011, cmd::overcurrent_status());
}

void test_LclCommander_status_flag() {
    cmd::off_all();
    std::tuple_element_t<0, LCLs>::is_on_invoked = false;
    std::tuple_element_t<1, LCLs>::is_on_invoked = false;
    std::tuple_element_t<2, LCLs>::is_on_invoked = false;
    std::tuple_element_t<3, LCLs>::is_on_invoked = false;

    TEST_ASSERT_EQUAL(0b0000, cmd::on_status());

    cmd::on(0);
    std::tuple_element_t<0, LCLs>::check_is_on_invocation();
    TEST_ASSERT_EQUAL(0b0001, cmd::on_status());

    cmd::on(7);
    std::tuple_element_t<2, LCLs>::check_is_on_invocation();
    TEST_ASSERT_EQUAL(0b0101, cmd::on_status());

    cmd::on(20);
    std::tuple_element_t<1, LCLs>::check_is_on_invocation();
    TEST_ASSERT_EQUAL(0b0111, cmd::on_status());

    cmd::on(0xFB);
    std::tuple_element_t<3, LCLs>::check_is_on_invocation();
    TEST_ASSERT_EQUAL(0b1111, cmd::on_status());

    cmd::off(7);
    std::tuple_element_t<2, LCLs>::check_is_on_invocation();
    TEST_ASSERT_EQUAL(0b1011, cmd::on_status());
}

void test_LclCommander() {
    UnityBegin("");

    RUN_TEST(test_LclCommander_notfound);
    RUN_TEST(test_LclCommander_init);
    RUN_TEST(test_LclCommander_on);
    RUN_TEST(test_LclCommander_off);
    RUN_TEST(test_LclCommander_overcurrent);
    RUN_TEST(test_LclCommander_status_flag);
    RUN_TEST(test_LclCommander_overcurrent_reset_by_on_off);

    UnityEnd();
}
