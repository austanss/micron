#pragma once
#include <stddef.h>
#include <stdint.h>

namespace util {
    class bitmap {
    public:
        size_t size;
        uint8_t* buffer;
        bool operator[](uint64_t index);
        void set(uint64_t index, bool value);
    };  
}