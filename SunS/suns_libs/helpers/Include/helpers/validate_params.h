#ifndef SUNS_SUNS_LIBS_HELPERS_INCLUDE_HELPERS_VALIDATE_PARAMS_H_
#define SUNS_SUNS_LIBS_HELPERS_INCLUDE_HELPERS_VALIDATE_PARAMS_H_

#include "logger.h"

namespace suns {
namespace helpers {
Telemetry::Params
validate_and_correct_params(std::uint8_t gain, std::uint8_t itime) {
    constexpr std::uint8_t default_gain  = 0;
    constexpr std::uint8_t default_itime = 10;
    constexpr std::uint8_t max_gain      = 3;
    constexpr std::uint8_t min_itime     = 1;

    Telemetry::Params valid_parameters;

    if (gain > max_gain) {
        valid_parameters.gain = default_gain;
        LOG_WARNING("[params] Setting default gain = 0");
    } else {
        valid_parameters.gain = gain;
    }

    if (itime < min_itime) {
        valid_parameters.itime = default_itime;
        LOG_WARNING("[params] Setting default itime = 10");
    } else {
        valid_parameters.itime = itime;
    }

    return valid_parameters;
}
}  // namespace helpers
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_HELPERS_INCLUDE_HELPERS_VALIDATE_PARAMS_H_
