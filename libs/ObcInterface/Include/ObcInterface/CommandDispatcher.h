#ifndef LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMANDDISPATCHER_H_
#define LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMANDDISPATCHER_H_

#include <cstdint>
#include <gsl/gsl>
#include "logger.h"

#include "Command.h"

/*!

 * @tparam
 */

/*!
 * Class used to dispatch commands from data received.
 * It assumes that command opcodes are above (higher value) than any data
 * readout. When command is received by the device @refitem parse method should
 * be invoked (e.g. in ISR). Later, in main thread @refitem dispatch should be
 * invoked.
 * @tparam Executor which will be invoked on match. In particular, is should
 * implement invoke method, such as: \code struct Executor { template<typename
 * Command> void invoke(Command& cmd, gsl::span<uint8_t> args);
 * };
 * \endcode
 * @tparam Types Types of commands. Every one of them should be inherited
 * from Command.
 */
template<class Executor, class... Types>
class CommandDispatcher : private Executor, private Types... {
 private:
    template<class T, class... T2>
    struct Crawler {
        constexpr static auto min_opcode =
            (Crawler<T2...>::min_opcode < T::opcode) ? Crawler<T2...>::min_opcode :
                                                       T::opcode;
        constexpr static auto max_params =
            (Crawler<T2...>::max_params > T::params) ? Crawler<T2...>::max_params :
                                                       T::params;
    };

    template<class T>
    struct Crawler<T> {
        constexpr static auto min_opcode = T::opcode;
        constexpr static auto max_params = T::params;
    };

 public:
    /*!
     * Default ctor. Every command and executor default constructors will be
     * invoked.
     */
    CommandDispatcher() = default;

    /*!
     * Specialise executor, commands will be created using default ctors.
     * @param executor Executor object to use.
     */
    CommandDispatcher(Executor executor) : Executor(std::move(executor)) {
    }
    /*!
     * Fully specialise executors and commands.
     * @param executor Executor object to use
     * @param types Command objects.
     */
    CommandDispatcher(Executor executor, Types... types)
        : Executor(std::move(executor)), Types{std::move(types)}... {
    }

    /*!
     * Maximal supported length of command. Longer commands passed cause runtime
     * error. It is calculated from the types of commands.
     */
    constexpr static uint8_t max_command_length =
        Crawler<Types...>::max_params + 1;

    /*!
     * Parse command. This function buffers passed command and saves it for
     * dispatching later. If previous command was not yet dispatched, command
     * will be ignored.
     * @param cmd Command as received from hardware (raw data from I2C bus).
     */
    void parse(gsl::span<const std::uint8_t> cmd) {
        if (0 == cmd.size() || cmd.size() > storage.size()) {
            LOG_ERROR("Wrong command length!");
            return;
        }

        if (cmd[0] < min_opcode) {
            // data readout
            return;
        }

        if (command_pending) {
            LOG_ERROR("Command dropped!");
        }

        std::copy(cmd.begin(), cmd.end(), storage.begin());
        length = cmd.size();

        command_pending = true;
    }

    /*!
     * Dispatches any pending command.
     */
    void dispatch() {
        if (command_pending == false) {
            return;
        }

        run_command<0, Types...>(
            storage[0], gsl::make_span(storage.data() + 1, length - 1));

        command_pending = false;
    }

 private:
    constexpr static uint8_t min_opcode = Crawler<Types...>::min_opcode;

    std::array<std::uint8_t, max_command_length> storage;
    std::uint8_t length;

    volatile bool command_pending = false;

    template<uint8_t index>
    void run_command(uint8_t opcode, const gsl::span<uint8_t>& args) {
        LOG_WARNING("No command for %d, %d", opcode, args.size());
    }

    template<uint8_t index, class Head, class... Tail>
    void run_command(uint8_t opcode, const gsl::span<uint8_t>& args) {
        if (Head::opcode == opcode && Head::params == args.size()) {
            Executor::invoke(static_cast<Head&>(*this), args);
        } else {
            run_command<index + 1, Tail...>(opcode, args);
        }
    }
};

#endif  // LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMANDDISPATCHER_H_
