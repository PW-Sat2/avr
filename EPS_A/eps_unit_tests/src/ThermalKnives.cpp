#include "ThermalKnives.h"
#include <hal/hal>
#include <queue>
#include "unity.h"

enum class PinOper {
    Set,
    Reset,
};

template<typename /*tag*/>
struct TK_pin {
    static hal::libs::FIFO_data<PinOper, 5> invocations;

    static void set() {
        invocations.append(PinOper::Set);
    }
    static void reset() {
        invocations.append(PinOper::Reset);
    }
};

template<typename tag>
hal::libs::FIFO_data<PinOper, 5> TK_pin<tag>::invocations;

using PinSail = TK_pin<std::true_type>;
using PinSads = TK_pin<std::false_type>;

using tk = avr::ThermalKnives<PinSail, PinSads>;

template<typename T>
void check(std::initializer_list<PinOper> operations) {
    TEST_ASSERT_EQUAL(operations.size(), T::invocations.getLength());

    for (auto now : operations) {
        TEST_ASSERT_EQUAL(now, T::invocations.get());
    }
}

void clear() {
    // clear burn timer
    for (int i = 0; i < 200; ++i) {
        tk::tick();
    }
    PinSail::invocations.flush();
    PinSads::invocations.flush();
}

void test_TK_BurnsDoesNothing() {
    tk::burn(tk::Types::Sail);
    check<PinSail>({});
    check<PinSads>({});

    tk::burn(tk::Types::Sads);
    check<PinSail>({});
    check<PinSads>({});

    clear();
}

void test_TK_BurnsTurnOn() {
    // enable SAIL
    tk::burn(tk::Types::Sail);
    tk::tick();
    check<PinSail>({PinOper::Set});
    check<PinSads>({PinOper::Reset});

    tk::tick();
    check<PinSail>({PinOper::Set});
    check<PinSads>({PinOper::Reset});

    // enable SADS
    tk::burn(tk::Types::Sads);
    tk::tick();
    check<PinSail>({PinOper::Set});
    check<PinSads>({PinOper::Set});

    tk::tick();
    check<PinSail>({PinOper::Set});
    check<PinSads>({PinOper::Set});

    clear();
}

void test_TK_BurnsTimeout() {
    // enable SAIL
    tk::burn(tk::Types::Sail);
    tk::tick();
    check<PinSail>({PinOper::Set});
    check<PinSads>({PinOper::Reset});

    // enable SADS one tick later
    tk::burn(tk::Types::Sads);
    tk::tick();
    check<PinSail>({PinOper::Set});
    check<PinSads>({PinOper::Set});

    auto timeout = 120 - 2;  // seconds
    while (--timeout) {
        tk::tick();
        check<PinSail>({PinOper::Set});
        check<PinSads>({PinOper::Set});
    }

    // SAIL auto-off
    tk::tick();
    check<PinSail>({PinOper::Set, PinOper::Reset});
    check<PinSads>({PinOper::Set});

    // SADS auto-off
    tk::tick();
    check<PinSail>({PinOper::Reset});
    check<PinSads>({PinOper::Set, PinOper::Reset});

    // both channels off
    tk::tick();
    check<PinSail>({PinOper::Reset});
    check<PinSads>({PinOper::Reset});
    tk::tick();
    check<PinSail>({PinOper::Reset});
    check<PinSads>({PinOper::Reset});
}

void test_TK_BurnsTimeoutExtendedByCommand() {
    tk::burn(tk::Types::Sail);
    tk::burn(tk::Types::Sads);

    // wait for 100 seconds
    for (int i = 0; i < 100; ++i) {
        tk::tick();
        check<PinSail>({PinOper::Set});
        check<PinSads>({PinOper::Set});
    }

    // re-enable
    tk::burn(tk::Types::Sail);
    tk::burn(tk::Types::Sads);

    auto timeout = 120;
    while (--timeout) {
        tk::tick();
        check<PinSail>({PinOper::Set});
        check<PinSads>({PinOper::Set});
    }

    tk::tick();
    check<PinSail>({PinOper::Set, PinOper::Reset});
    check<PinSads>({PinOper::Set, PinOper::Reset});

    tk::tick();
    check<PinSail>({PinOper::Reset});
    check<PinSads>({PinOper::Reset});
}

void test_TK() {
    UnityBegin("");

    RUN_TEST(test_TK_BurnsDoesNothing);
    RUN_TEST(test_TK_BurnsTurnOn);
    RUN_TEST(test_TK_BurnsTimeout);
    RUN_TEST(test_TK_BurnsTimeoutExtendedByCommand);

    UnityEnd();
}
