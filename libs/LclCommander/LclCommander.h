#ifndef LIBS_LCLCOMMANDER_LCLCOMMANDER_H_
#define LIBS_LCLCOMMANDER_LCLCOMMANDER_H_

#include <bitset>
#include <tuple>

#include "LclInterface.h"

namespace eps {

/*!
 * Commander module for LCLs. Should be shared between modules to turn
 * on/off/check LCLs
 * @tparam LCLs std::tuple containing all LclInterfaces.
 */
template<typename LCLs>
class LclCommander {
 public:
    /*!
     * Initialises all LCLs (set proper GPIO modes)
     */
    static void init() {
        hal::libs::for_each_tuple_type<InitExec, LCLs>();
    }

    /*!
     * Turn ON LCL with specified ID. If LCL is not found with this ID error log
     * will be printed.
     * @param id Numeric value of LCL ID.
     */
    static void on(uint8_t id) {
        Dispatch<OnExec, LCLs>::dispatch(id);
    }

    /*!
     * Turn OFF LCL with specified ID. If LCL is not found with this ID error
     * log will be printed.
     * @param id Numeric value of LCL ID.
     */
    static void off(uint8_t id) {
        Dispatch<OffExec, LCLs>::dispatch(id);
    }

    /*!
     * Turn OFF all LCLs.
     */
    static void off_all() {
        hal::libs::for_each_tuple_type<OffExec, LCLs>();
    }


    static void handle_overcurrent() {
        hal::libs::for_each_tuple_type<CheckOvercurrentExec, LCLs>();
    }

    static uint8_t overcurrent_status() {
        return status_overcurrent.to_ulong();
    }

    static uint8_t on_status() {
        status_on.reset();
        hal::libs::for_each_tuple_type<CheckOnExec, LCLs>();
        return status_on.to_ulong();
    }

 private:
    static std::bitset<std::tuple_size<LCLs>::value> status_overcurrent, status_on;

    template<template<typename> typename Exec, typename Tuple>
    struct Dispatch {
        template<int index, int tag>
        struct Do {
            static void run(uint8_t id) {
                using Now = std::tuple_element_t<index, Tuple>;
                if (num(Now::id) == id) {
                    Exec<Now>::run();
                } else {
                    Do<index + 1, tag>::run(id);
                }
            }
        };

        template<int tag>
        struct Do<std::tuple_size<Tuple>::value, tag> {
            static void run(uint8_t id) {
                LOG_ERROR("Can't find LCL with id %d!\n", id);
            }
        };

        static void dispatch(uint8_t id) {
            Do<0, 0>::run(id);
        }
    };

    template<typename Lcl>
    struct InitExec {
        static void run() {
            Lcl::init();
            status_overcurrent[Lcl::bit_pos] = true;
        }
    };

    template<typename Lcl>
    struct OnExec {
        static void run() {
            Lcl::on();
            status_overcurrent[Lcl::bit_pos] = true;
        }
    };

    template<typename Lcl>
    struct OffExec {
        static void run() {
            Lcl::off();
            status_overcurrent[Lcl::bit_pos] = true;
        }
    };

    template<typename Lcl>
    struct CheckOvercurrentExec {
        static void run() {
            if (Lcl::overcurrent()) {
                Lcl::off();
                LOG_ERROR("[LCL] Overcurrent %s", Lcl::name);
                status_overcurrent[Lcl::bit_pos] = false;
            }
        }
    };

    template<typename Lcl>
    struct CheckOnExec {
        static void run() {
            status_on[Lcl::bit_pos] = Lcl::is_on();
        }
    };
};

template<typename LCLs>
std::bitset<std::tuple_size<LCLs>::value> LclCommander<LCLs>::status_overcurrent;

template<typename LCLs>
std::bitset<std::tuple_size<LCLs>::value> LclCommander<LCLs>::status_on;

}  // namespace eps

#endif  // LIBS_LCLCOMMANDER_LCLCOMMANDER_H_
