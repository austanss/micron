#include "kernel/bitmap.hxx"

bool util::bitmap::operator[](uint64_t index) {

    if (index > size * 8 - 1)
        asm volatile ("int $0x0d");

    uint64_t byte_index = index / 8;
    uint8_t bit_index = index % 8;
    uint8_t bit_indexer = 0b10000000 >> bit_index;

    if ((buffer[byte_index] & bit_indexer) > 0)
        return true;
        
    return false;
}

void util::bitmap::set(uint64_t index, bool value) {

    uint64_t byte_index = index / 8;
    uint8_t bit_index = index % 8;
    uint8_t bit_indexer = 0b10000000 >> bit_index;
    buffer[byte_index] &= ~bit_indexer;

    if (value)
        buffer[byte_index] |= bit_indexer;
}