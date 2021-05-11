#pragma once
#include "types.h"

namespace sys {
    namespace evsys {

        struct evsys_packet {
            uint32  signature; // 0x50535645 "EVSP"
            uint16  event_id;
            uint16  reserved;
            uint64  packet_size; // 16 + packet payload data size
        };

        struct evsys_recipient_descriptor {
            void    (*handler)(evsys_packet *);
            uint32  id_list_entries;
            uint16  reserved;
            uint16* id_list;
        };

        void    fire_event(uint16 evid, void* payload, uint32 nbytes);
        void    register_recipient(evsys_recipient_descriptor* recipient_descriptor);
        void    deregister_recipient(evsys_recipient_descriptor* recipient_descriptor);
    }
}