#ifndef LIBS_TELEMETRYBUFFER_INCLUDE_TELEMETRYBUFFER_TELEMETRYBUFFER_H_
#define LIBS_TELEMETRYBUFFER_INCLUDE_TELEMETRYBUFFER_TELEMETRYBUFFER_H_

#include <util/atomic.h>
#include <gsl/gsl>

template<typename T>
class TelemetryBuffer {
 public:
    TelemetryBuffer(void (*set_memory)(gsl::not_null<T*> tm))
        : set_memory(set_memory),
          actual(&data[0]),
          buffered(&data[1]),
          actual_index(0) {
    }
    void update() {
        printf("Swap buffers!\n");
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            buffered = &data[actual_index];
            next_index();
            actual = &data[actual_index];
            set_memory(actual);
        }

        *buffered = *actual;
    }

    void (*set_memory)(gsl::not_null<T*> tm);
    T *actual, *buffered;

    void next_index() {
        Expects(actual_index <= 1);
        constexpr std::uint8_t swap[2] = {1, 0};
        actual_index                   = swap[actual_index];
    }
    int actual_index;
    T data[2];
};

#endif  // LIBS_TELEMETRYBUFFER_INCLUDE_TELEMETRYBUFFER_TELEMETRYBUFFER_H_
