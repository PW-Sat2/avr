#ifndef LIBS_TWOSTATEFSM_TWOSTATEFSM_H_
#define LIBS_TWOSTATEFSM_TWOSTATEFSM_H_

#include <functional>

namespace avr {

/*!
 * Simple two-state Finite State Machine.
 * This machine have only two states (active and standby).
 * Switching between those states is controlled by active state -
 *  - when state's function should_change() will return true,
 *  than the state will change to alternate.
 *
 * action() method of State is invoked during every pass.
 * @tparam Params parameters to pass to should_change() method of active state.
 */
template<typename... Params>
class TwoStateFsm {
 public:
    /*!
     * State interface.
     * States should inherit and implement those methods.
     */
    struct IState {
        /*!
         * This method will be invoked with every tick()
         */
        virtual void action() = 0;
        /*!
         * When it return true, state will be altered.
         * @param t passed parameters from tick
         * @return whether state should be altered
         */
        virtual bool should_change(Params... t) = 0;
    };

    /*!
     * Ctor. References to active and standby states.
     * @param active Active state during first run.
     * @param standby Standby state during first run.
     */
    TwoStateFsm(IState& active, IState& standby)
        : active(active), standby(standby) {
    }

    /*!
     * Update FSM state and then run proper action().
     * @param t Values to pass to should_change() method.
     */
    void tick(Params... t) {
        if (active.get().should_change(t...)) {
            std::swap(active, standby);
        }

        active.get().action();
    }

 private:
    std::reference_wrapper<IState> active, standby;
};

}  // namespace avr

#endif  // LIBS_TWOSTATEFSM_TWOSTATEFSM_H_
