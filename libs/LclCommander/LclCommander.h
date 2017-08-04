#ifndef LIBS_LCLCOMMANDER_LCLCOMMANDER_H_
#define LIBS_LCLCOMMANDER_LCLCOMMANDER_H_

#include <tuple>

#include "LclInterface.h"

namespace eps {

/*!
 * Commander module for LCLs. Should be shared between modules to turn on/off/check LCLs
 * @tparam LCLs std::tuple containing all LclInterfaces.
 */
template<typename LCLs>
class LclCommander {
public:
    /*!
     * Turn ON LCL with specified ID. If LCL is not found with this ID error log will be printed.
     * @param id Numeric value of LCL ID.
     */
    static void on(uint8_t id) {
        Dispatch<OnExec, LCLs>::dispatch(id);
    }

    /*!
     * Turn OFF LCL with specified ID. If LCL is not found with this ID error log will be printed.
     * @param id Numeric value of LCL ID.
     */
    static void off(uint8_t id) {
        Dispatch<OffExec, LCLs>::dispatch(id);
    }

private:
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
    struct OnExec {
        static void run() {
            Lcl::on();
        }
    };

    template<typename Lcl>
    struct OffExec {
        static void run() {
            Lcl::off();
        }
    };
};


}  // namespace eps

#endif  // LIBS_LCLCOMMANDER_LCLCOMMANDER_H_
