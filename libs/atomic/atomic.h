#ifndef LIBS_ATOMIC_ATOMIC_H_
#define LIBS_ATOMIC_ATOMIC_H_

#include <util/atomic.h>

namespace avr {

template<class T>
class Atomic {
 public:
    void operator=(const T& rhs) {
        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            value = rhs;
        }
    }

    constexpr operator T() const {
        return value;
    }

    constexpr const T& get() const {
        return value;
    }

 private:
    T value;
};

}  // namespace avr

#endif  // LIBS_ATOMIC_ATOMIC_H_
