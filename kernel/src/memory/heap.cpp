#include "heap.h"
#include "pmm.h"

void* heap_start;
void* heap_end;

void memory::heap::initialize_heap(void* heap_address, uint64 heap_length) {
    heap_start = heap_address;
    heap_end = (void*)((uint64)heap_start + heap_length);
    heap_segment_header* start_segment = (heap_segment_header*)heap_address;
    start_segment->length = heap_length - sizeof(heap_segment_header);
    start_segment->last = NULL;
    start_segment->next = NULL;
    start_segment->free = true;
}

void* memory::heap::malloc(uint64 size) {
    if (size == 0) return nullptr;
    if (size % 0x10 != 0) size = size % 0x10 + 0x10; //Make sure to round up to 0x10

    heap_segment_header* current_segment = (heap_segment_header*)heap_start;
    while (true) {
        if (current_segment->free) {
            
            if (current_segment->length > size) {
                current_segment->split(size);
                current_segment->free = false;
                return current_segment;
            }
            if (current_segment->length == size ) {
                current_segment->free = false;
                return current_segment;
            }
        }
        if (current_segment->next == NULL) break;
        current_segment = current_segment->next;
    }
    return nullptr;
}

void memory::heap::free(void* addr) {
    heap_segment_header* current_segment = (heap_segment_header*)addr;
    current_segment->free = true;
    if (current_segment->next != NULL && current_segment->next->free)current_segment->combine_forward();
    if (current_segment->last != NULL && current_segment->last->free)current_segment->combine_backward();
}

memory::heap::heap_segment_header* memory::heap::heap_segment_header::split(uint64 split_length) {
    if (split_length < 0x10) return NULL;
    uint64 splitSegLength = length - split_length - (sizeof(heap_segment_header) * 2);
    if (splitSegLength< 0x10) return NULL;
    heap_segment_header* new_split_header = (heap_segment_header*) ((uint64)this + split_length + sizeof(heap_segment_header));
    next->last = new_split_header; // Set the next segment's last segment to our new segment
    new_split_header->next = next; // Set the new segment's next segment to our original next segment
    next = new_split_header; // Set our next segment to our new segment
    new_split_header->last = this; // Set our new segment's last segment to the currect segment
    new_split_header->length = splitSegLength;
    new_split_header->free = free; // Make sure the new segment is as free as the original
    length = split_length;
    return new_split_header;
}

void memory::heap::heap_segment_header::combine_forward() {
    if (next == NULL) return;
    if (!next->free) return;
    
    if (next->next != NULL)
        next->next->last = this;

    length = length + next->length + sizeof(heap_segment_header);

    next = next->next; 
}

void memory::heap::heap_segment_header::combine_backward() {
    if (last != NULL) last->combine_forward();
}
