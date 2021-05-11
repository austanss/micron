#pragma once
#include <stdint.h>

struct evsys_packet {
    uint32_t    signature; // 0x50535645 "EVSP"
    uint16_t    event_id;
    uint16_t    reserved;
    uint64_t    packet_size; // 16 + packet payload data size
};

struct evsys_recipient_descriptor {
    void        (*handler)(evsys_packet *);
    uint32_t    id_list_entries;
    uint16_t    reserved;
    uint16_t*   id_list;
};