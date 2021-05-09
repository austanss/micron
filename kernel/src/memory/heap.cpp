#include "heap.h"
#include "pmm.h"
#include "paging.h"
 
void* heap_start;
void* heap_end;
memory::heap::heap_segment_header* last_segment;

void memory::heap::initialize_heap(void* heap_address, uint64 heap_pages) {
    heap_start = heap_address;

    for (uint i = 0; i < heap_pages; i++)
        memory::paging::map_memory(heap_start + (i * 0x1000), memory::pmm::request_page(), false);

    heap_segment_header* start_segment = (heap_segment_header*)heap_start;
    
    heap_end = heap_start + heap_pages * 0x1000;

    start_segment->length = heap_pages * 0x1000 - sizeof(heap_segment_header);
    start_segment->last = NULL;
    start_segment->next = NULL;
    start_segment->free = true;

    last_segment = start_segment;
}

void* memory::heap::malloc(uint64 size) {
    if (size == 0) return nullptr;
    if (size % 0x10 != 0) size = size + (0x10 - (size % 0x10)); //Make sure to round up to 0x10

    heap_segment_header* current_segment = (heap_segment_header*)heap_start;
    while (true) {
        if (current_segment->free) {
            if (current_segment->length > size) {
                current_segment->split(size);
                current_segment->free = false;
                return (void*)((uint64)current_segment + sizeof(heap_segment_header));
            }

            if (current_segment->length == size) {
                current_segment->free = false;
                return (void*)((uint64)current_segment + sizeof(heap_segment_header));
            }
        }

        if (current_segment->next == NULL) break;
        current_segment = current_segment->next;
    }
    expand_heap(size);
    return nullptr;
}

void memory::heap::free(void* addr) {
    heap_segment_header* current_segment = (heap_segment_header*)addr - 1;
    current_segment->free = true;
    current_segment->combine_forward();
    current_segment->combine_backward();
}

memory::heap::heap_segment_header* memory::heap::heap_segment_header::split(uint64 split_length) {
    if (split_length < 0x10) return NULL;
    sint64 split_segment_length = length - split_length - sizeof(heap_segment_header);
    if (split_segment_length < 0x10) return NULL;
    heap_segment_header* new_split_header = (heap_segment_header*)((uint64)this + split_length + sizeof(heap_segment_header));
    next->last = new_split_header; // Set the next segment's last segment to our new segment
    new_split_header->next = next; // Set the new segment's next segment to our original next segment
    next = new_split_header; // Set our next segment to our new segment
    new_split_header->last = this; // Set our new segment's last segment to the currect segment
    new_split_header->length = split_segment_length;
    new_split_header->free = free; // Make sure the new segment is as free as the original
    length = split_length;

    last_segment = new_split_header;

    return new_split_header;
}

void memory::heap::expand_heap(uint64 length) {
    if (length % 0x1000) {
        length -= length % 0x1000;
        length += 0x1000;
    }

    uint64 page_count = length / 0x1000;
    heap_segment_header* new_segment = (heap_segment_header *)heap_end;

    for (size_t i = 0; i < page_count; i++){
        memory::paging::map_memory(heap_end, memory::pmm::request_page(), false);
        heap_end = (void*)((size_t)heap_end + 0x1000);
    }

    new_segment->free = true;
    new_segment->last = last_segment;
    last_segment->next = new_segment;
    last_segment = new_segment;
    new_segment->next = NULL;
    new_segment->length = length - sizeof(heap_segment_header);
    new_segment->combine_backward();
}

void memory::heap::heap_segment_header::combine_forward() {
    if (!next) return;
    if (!next->free) return;
    
    if (next == last_segment) last_segment = this;

    if (next->next != NULL)
        next->next->last = this;

    length = length + next->length + sizeof(heap_segment_header);

    next = next->next; 
}

void memory::heap::heap_segment_header::combine_backward() {
    if (last != NULL) last->combine_forward();
}
