#include "evsys.h"
#include "memory/heap.h"
#include "memory/operations.h"
#include "memory/pmm.h"

bool evsys_ready = false;

struct descriptor_list_node {
    sys::evsys::evsys_recipient_descriptor* descriptor;
    descriptor_list_node*                   next;
};

descriptor_list_node evsys_list_root {
    nullptr,
    nullptr
};

extern "C" void ring3_call(void (*function)(sys::evsys::evsys_packet*), sys::evsys::evsys_packet* packet, void* stack);

void sys::evsys::fire_event(uint16 evid, void* payload, uint32 nbytes)
{
    if (!evsys_ready)
        return;

    descriptor_list_node* walker;

    for (walker = evsys_list_root.next; !!walker; walker = walker->next)
    {
        bool evid_detected = false;

        for (uint i = 0; i < walker->descriptor->id_list_entries; i++)
        {
            if (walker->descriptor->id_list[i] == evid)
            {
                evid_detected = true;
                break;
            }
        }

        if (!evid_detected)
            continue;

        if (walker->descriptor->handler == nullptr)
        {
            deregister_recipient(walker->descriptor);
            continue;
        }

        if (walker->descriptor->reserved != 0)
        {
            deregister_recipient(walker->descriptor);
            continue;
        }

        evsys_packet* packet = (evsys_packet *)memory::heap::malloc(sizeof(evsys_packet) + nbytes);

        packet->signature = 0x50535645;
        packet->event_id = evid;
        packet->reserved = 0;
        packet->packet_size = sizeof(evsys_packet) + nbytes;
        memory::operations::memcpy((void *)((address)packet + sizeof(packet)), payload, nbytes);

        void* handler_stack = memory::pmm::request_pool(4);

        ring3_call(walker->descriptor->handler, packet, handler_stack);

        memory::heap::free(packet);
        memory::pmm::free_pool(handler_stack);
    }

    if (!walker)
        return;
} 

void sys::evsys::register_recipient(evsys_recipient_descriptor* recipient_descriptor)
{
    if (!evsys_ready)
        return;
    
    descriptor_list_node* walker;

    for (walker = &evsys_list_root; !!walker->next; walker = walker->next);

    if (!walker)
        return;

    walker->next = (descriptor_list_node *)memory::heap::malloc(sizeof(descriptor_list_node));

    walker = walker->next;

    walker->descriptor = recipient_descriptor;

    walker->next = nullptr;
}

void sys::evsys::deregister_recipient(evsys_recipient_descriptor* recipient_descriptor)
{
    if (!evsys_ready)
        return;
          
    descriptor_list_node* walker;

    for (walker = &evsys_list_root; !!walker->next; walker = walker->next)
        if (walker->next->descriptor == recipient_descriptor)
            break;

    if (!walker->next)
        return;

    descriptor_list_node* ahead = walker->next->next;

    memory::heap::free(walker->next);

    walker->next = ahead;
}

extern "C" void evsys_initialize()
{
    evsys_ready = true;
}