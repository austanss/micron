#include "util/bitmap.h"

bool util::bitmap::operator[](uint64 index) {
    return get(index);
}

bool util::bitmap::get(uint64 index) {

    uint byte_index = index / 8;
    uint08 bit_index = index % 8;
    uint08 bit_indexer = 0b10000000 >> bit_index;

    return (buffer[byte_index] & bit_indexer);
}

void util::bitmap::set(uint64 index, bool value) {

    uint64 byte_index = index / 8;
    uint08 bit_index = index % 8;
    uint08 bit_indexer = 0b10000000 >> bit_index;
    buffer[byte_index] &= ~bit_indexer;

    if (value)
        buffer[byte_index] |= bit_indexer;
}