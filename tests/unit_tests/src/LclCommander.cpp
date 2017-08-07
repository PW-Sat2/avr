#include <unity.h>
#include <hal/hal>

#include "LclCommander.h"

using namespace eps;

template<int id_>
struct LCL {
    enum class Id : std::uint8_t {};

    using IdType               = Id;
    static constexpr IdType id = static_cast<Id>(id_);

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
};
template<int id_>
bool LCL<id_>::state_on;
template<int id_>
bool LCL<id_>::initialised;

using LCLs = std::tuple<LCL<0>, LCL<20>, LCL<7>, LCL<0xFB>>;
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

void test_LclCommander() {
    UnityBegin("");

    RUN_TEST(test_LclCommander_notfound);
    RUN_TEST(test_LclCommander_init);
    RUN_TEST(test_LclCommander_on);
    RUN_TEST(test_LclCommander_off);

    UnityEnd();
}
