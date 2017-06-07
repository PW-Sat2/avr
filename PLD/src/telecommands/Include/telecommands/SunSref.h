#ifndef PLD_SRC_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_
#define PLD_SRC_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_

#include <ObcInterface/CommandDispatcher.h>

namespace PLD {
namespace telecommands {

class SunSRef : public CommandWithOpcode<0x80, 0> {
 public:
    void invoke(gsl::span<std::uint8_t>) override {
        printf("SunS\n");
    }
};

}  // namespace telecommands
}  // namespace PLD

#endif  // PLD_SRC_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_
