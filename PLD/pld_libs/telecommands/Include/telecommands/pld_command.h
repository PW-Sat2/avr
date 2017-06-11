#ifndef PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PLD_COMMAND_H_
#define PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PLD_COMMAND_H_

#include <ObcInterface/CommandDispatcher.h>
#include <hardware/interface.h>
#include <cstdint>

namespace pld {
namespace telecommands {

template<std::uint8_t opcode>
class PldCommand : public CommandWithOpcode<opcode, 0> {
 public:
    PldCommand(hardware::Interface*& hardware) : hardware(hardware) {
    }

    void invoke(gsl::span<const std::uint8_t> params) override {
        hardware->obc_interrupt_set();
        this->work(params);
    }

    virtual void work(gsl::span<const std::uint8_t> params) = 0;

    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PLD_COMMAND_H_
