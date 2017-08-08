#ifndef LIBS_REDUNDANCY_REDUNDANCY_H_
#define LIBS_REDUNDANCY_REDUNDANCY_H_

#include <type_traits>

namespace avr {
namespace redundancy {

/**
 * @brief Performs bitwise majority vote based on three inputs.
 * @param[in] a First input
 * @param[in] b Second input
 * @param[in] c Third input
 * @tparam T Type used for majority vote. Must be integral or enum type
 * @return Value calculated from bitwise majority vote.
 */
template<typename T>
T Correct(T a, T b, T c) {
    static_assert(std::is_integral<T>::value || std::is_enum<T>::value,
                  "Correction requires integral or enum type");
    return (a & b) | (b & c) | (a & c);
}

/**
 * @brief Performs bitwise majority vote based on five inputs.
 * @param[in] a First input
 * @param[in] b Second input
 * @param[in] c Third input
 * @param[in] d Fourth input
 * @param[in] e Fifth input
 * @tparam T Type used for majority vote. Must be integral or enum type
 * @return Value calculated from bitwise majority vote.
 */
template<typename T>
T Correct(T a, T b, T c, T d, T e) {
    static_assert(std::is_integral<T>::value || std::is_enum<T>::value,
                  "Correction requires integral or enum type");
    return (c & d & e) | (b & d & e) | (b & c & e) | (b & c & d) | (a & d & e) |
           (a & c & e) | (a & c & d) | (a & b & e) | (a & b & d) | (a & b & c);
}

/**
 * @brief Performs bitwise majority vote based on three elements in array.
 * @param[in] elements Array with three elements
 * @tparam T Type used for majority vote. Must be integral or enum type
 * @return Value calculated from bitwise majority vote.
 */
template<typename T>
inline T Correct(std::array<T, 3>& elements) {
    return Correct(elements[0], elements[1], elements[2]);
}

/**
 * @brief Performs bitwise majority vote based on five elements in array.
 * @param[in] elements Array with five elements
 * @tparam T Type used for majority vote. Must be integral or enum type
 * @return Value calculated from bitwise majority vote.
 */
template<typename T>
inline T Correct(std::array<T, 5>& elements) {
    return Correct(elements[0], elements[1], elements[2], elements[3], elements[4]);
}

}  // namespace redundancy
}  // namespace avr

#endif  // LIBS_REDUNDANCY_REDUNDANCY_H_
