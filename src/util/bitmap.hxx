#pragma once
#include "types.hxx"

namespace util {
    class bitmap {
    public:
        uint size;
        byte* buffer;
        bool operator[](uint64 index);
        bool get(uint64 index);
        void set(uint64 index, bool value);
    };  
}