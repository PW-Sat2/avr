#ifndef LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMANDDISPATCHER_H_
#define LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMANDDISPATCHER_H_

#include <logger.h>
#include <cstdint>
#include <gsl/gsl>

/*!
 * Command interface. Every telecommand should implement it.
 */
class Command {
 public:
    /*!
     * Callback invoked when telecommand is received.
     * It can be assumed that correct number of arguments will be passed.
     * @param params Parameters passed by OBC.
     */
    virtual void invoke(gsl::span<const std::uint8_t> params) = 0;
    virtual std::uint8_t opcode()                             = 0;
    virtual std::uint8_t params()                             = 0;
};

/*!
 * Static implementation of opcode and parameters number resolution.
 * Every telecommand should be a child of this class.
 * @tparam command_opcode Opcode of a telecommand.
 * @tparam command_params Expected number of parameters.
 */
template<std::uint8_t command_opcode, std::uint8_t command_params>
class CommandWithOpcode : public Command {
 public:
    constexpr static std::uint8_t opcode_ = command_opcode;
    constexpr static std::uint8_t params_ = command_params;

    virtual std::uint8_t opcode() override final {
        return opcode_;
    }
    virtual std::uint8_t params() override final {
        return params_;
    }
};

namespace details {
template<class T, class... T2>
struct Crawler {
    constexpr static auto min_opcode = (Crawler<T2...>::min_opcode < T::opcode_) ?
                                           Crawler<T2...>::min_opcode :
                                           T::opcode_;
    constexpr static auto max_params = (Crawler<T2...>::max_params > T::params_) ?
                                           Crawler<T2...>::max_params :
                                           T::params_;
};

template<class T>
struct Crawler<T> {
    constexpr static auto min_opcode = T::opcode_;
    constexpr static auto max_params = T::params_;
};
}  // namespace details

/*!
 * Class used to dispatch commands from data received.
 * It assumes that command opcodes are above (higher value) than any data
 * readout. When command is received by the device @refitem parse method should
 * be invoked (e.g. in ISR). Later, in main thread @refitem dispatch should be
 * invoked.
 * @tparam types Types of telecommands. They should be inherited from
 * CommandWithOpcode.
 */
template<class... types>
class CommandDispatcher {
 public:
    /*!
     * Maximal supported length of command. Longer commands passed cause runtime
     * error. It is derived from types of telecommands.
     */
    constexpr static uint8_t max_command_length =
        details::Crawler<types...>::max_params + 1;

    /*!
     * Default ctor. Only for testing.
     */
    [[deprecated]] CommandDispatcher() {
    }

    /*!
     * Creates command list for later dispatching.
     * @param command_list List of supported commands.
     */
    CommandDispatcher(std::initializer_list<Command*> command_list) {
        Expects(command_list.size() == commands.size());
        std::copy(command_list.begin(), command_list.end(), commands.begin());
    }

    /*!
     * Parse command. This function buffers passed command and saves it for
     * later dispatching. If previous command was not yet dispatched, command
     * will be ignored.
     * @param cmd Command as received from hardware.
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
        data = gsl::make_span(storage.data(), cmd.size());

        command_pending = true;
    }

    /*!
     * Dispatches any pending command.
     */
    void dispatch() {
        if (command_pending == false) {
            return;
        }
        Command** cmd =
            std::find_if(commands.begin(), commands.end(), [=](Command* _) {
                return (_->opcode() == data[0]) &&
                       (_->params() == data.size() - 1);
            });
        if (cmd != commands.end()) {
            (*cmd)->invoke(data.subspan(1));
        } else {
            LOG_WARNING("No command for %d, %d", data[0], data.size() - 1);
        }
        command_pending = false;
    }

 private:
    std::array<Command*, sizeof...(types)> commands;
    constexpr static uint8_t min_opcode = details::Crawler<types...>::min_opcode;

    std::array<std::uint8_t, max_command_length> storage;
    gsl::span<std::uint8_t> data;

    volatile bool command_pending = false;
};

#endif  // LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMANDDISPATCHER_H_
