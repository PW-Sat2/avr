#ifndef LIBS_LCLINTERFACE_LCLINTERFACE_H_
#define LIBS_LCLINTERFACE_LCLINTERFACE_H_

#include <logger.h>
#include <hal/hal>

namespace eps {

/*!
 * EPS interface to LCL.
 * Intended to use as a glue layer between FPF device driver and user code.
 * @tparam PinOn DigitalIO pin ON.
 * @tparam PinFlagB DigitalIO pin FlagB.
 * @tparam Enum Type containing possible enum IDs (e.g. enum class)
 * @tparam id_ ID of this LCL
 * @tparam name_ Parameter pack, containing name of LCL.
 */
template<typename PinOn, typename PinFlagB, typename Enum, Enum id_, char... name_>
class LclInterface {
 public:
    /*!
     * Type of ID
     */
    using IdType = Enum;

    /*!
     * ID of this LCL
     */
    static constexpr IdType id = id_;

    /*!
     * Name of this LCL
     */
    static constexpr char name[] = {name_..., '\0'};

    /*!
     * Initialises LCL and log message
     */
    static void init() {
        LOG_INFO("[LCL] INIT %s\n", name);
        FPF::init();
    }

    /*!
     * Turn on LCL and log message
     */
    static void on() {
        LOG_INFO("[LCL] ON %s\n", name);
        FPF::on();
    }

    /*!
     * Turn off LCL and log message
     */
    static void off() {
        LOG_INFO("[LCL] OFF %s\n", name);
        FPF::off();
    }

 private:
    using FPF =
        hal::devices::FPF270x<PinOn, PinFlagB, hal::DigitalIO::Dummy<false>>;
};

template<typename PinOn, typename PinFlagB, typename Enum, Enum id_, char... name_>
constexpr char LclInterface<PinOn, PinFlagB, Enum, id_, name_...>::name[];

}  // namespace eps

#endif  // LIBS_LCLINTERFACE_LCLINTERFACE_H_
