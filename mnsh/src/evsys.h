#pragma once

struct evsys_packet {
    unsigned int    signature; // 0x50535645 "EVSP"
    unsigned short    event_id;
    unsigned short    reserved;
    unsigned long    packet_size; // 16 + packet payload data size
};

struct evsys_recipient_descriptor {
    void        (*handler)(evsys_packet *);
    unsigned int    id_list_entries;
    unsigned short    reserved;
    unsigned short*   id_list;
};

extern evsys_recipient_descriptor evrd;

void _levrd(evsys_recipient_descriptor* tblevrd);

inline void enable_evsys_handling()
{
    _levrd(&evrd);
}