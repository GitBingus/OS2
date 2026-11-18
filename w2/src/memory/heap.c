#include "heap.h"
#include "types.h"
#include "kbool.h"
#include "tui.h"

#define HEAP_SIZE (64 * 1024)

static u8int heap_memory[HEAP_SIZE]
    __attribute__((aligned(16)));

static heap_block_t *heap_head = 0;

static void heap_split_block(heap_block_t *block, u32int size);

void heap_init(void) {
    // Point heap_head to the beginning of heap_memory.
    heap_head = (heap_block_t *)&heap_memory[0];

    // Set the block's size to the total heap size minus the size of the heap_block_t structure.
    heap_head->size = HEAP_SIZE - sizeof(heap_block_t);
    // Mark the block as free.
    heap_head->free = true;
    // Set the next pointer to NULL.
    heap_head->next = 0;
}

void *kmalloc(u32int size) {
    if (size == 0 || heap_head == 0)
        return 0;

    if (size > 0xFFFFFFFFu - 7) // Prevent overflow when rounding up to the nearest multiple of 8
        return 0;

    size = (size + 7u) & ~7u; // Round up to the nearest multiple of 8

    heap_block_t *current = heap_head;

    while (current != 0) {
        // Check whether current is free.
        if (current->free && current->size >= size) {
            heap_split_block(current, size);

            current->free = false;

            return (void *)(current + 1);
        }

        current = current->next;
    }

    return 0; // No suitable block found.
}


void kfree(void *ptr) {
    if (ptr == 0 || heap_head == 0)
        return;

    heap_block_t *block = heap_head; // Start from the head of the heap

    // Find the block corresponding to the pointer
    while (block != 0) {
        if ((void *)(block + 1) == ptr) {
            break;
        }

        block = block->next;
    }

    // if (block == 0 || block->free)
    //     return;

    if (block == 0) {
        tui_write("[DEBUG] Block not found\n");
        return;
    }

    if (block->free) {
        tui_write("[DEBUG] Block already free\n");
        return;
    }
    
    tui_write("[DEBUG] Freeing block\n");
    block->free = true;

    // Coalesce adjacent free blocks.
    heap_block_t *current = heap_head;

    // Iterate through the heap and merge adjacent free blocks.
    while (current != 0) {
        if (current->free && current->next != 0 && current->next->free) {
            current->size += sizeof(heap_block_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}


static void heap_split_block(heap_block_t *block, u32int size) {
    // Ensure the block can satisfy the allocation.
    if (size > block->size)
        return;

    u32int remaining = block->size - size;

    // Need enough space for a header and another
    // minimum-sized allocation
    if (remaining < sizeof(heap_block_t) + 8)
        return;

    // Locate the new block header after the payload
    heap_block_t *new_block = (heap_block_t *)(
        (u8int *)(block + 1) + size
    );

    // Initialise the remaining free block
    new_block->size = remaining - sizeof(heap_block_t);

    new_block->free = true;
    new_block->next = block->next;

    // Resize the original block and link the new one
    block->size = size;
    block->next = new_block;
}
