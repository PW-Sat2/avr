#include <hal/hal>
#include "unity.h"

#include "TwoStateFsm.h"

using namespace avr;

using FSM = TwoStateFsm<>;

struct MockState : FSM::IState {
    bool change;
    bool invoked;

    void action() override {
        invoked = true;
    }

    bool was_invoked() {
        auto hold = invoked;
        invoked   = false;
        return hold;
    }

    bool should_change() override {
        return change;
    }
};

MockState stateA, stateB;
FSM fsm(stateA, stateB);

void set_changes(bool A, bool B) {
    stateA.change = A;
    stateB.change = B;
}

void check_invoked(bool A, bool B) {
    TEST_ASSERT_EQUAL(A, stateA.was_invoked());
    TEST_ASSERT_EQUAL(B, stateB.was_invoked());
}

void test_TwoStateFsm_default_state() {
    check_invoked(false, false);
    set_changes(false, false);

    fsm.tick();

    check_invoked(true, false);
}

void test_TwoStateFsm_transitions() {
    check_invoked(false, false);
    set_changes(false, false);
    fsm.tick();
    check_invoked(true, false);

    set_changes(true, false);
    fsm.tick();
    check_invoked(false, true);

    set_changes(false, false);
    fsm.tick();
    check_invoked(false, true);

    set_changes(false, true);
    fsm.tick();
    check_invoked(true, false);

    fsm.tick();
    check_invoked(true, false);

    set_changes(true, true);
    fsm.tick();
    check_invoked(false, true);

    fsm.tick();
    check_invoked(true, false);
}

void test_TwoStateFsm() {
    UnityBegin("");

    RUN_TEST(test_TwoStateFsm_default_state);
    RUN_TEST(test_TwoStateFsm_transitions);

    UnityEnd();
}
