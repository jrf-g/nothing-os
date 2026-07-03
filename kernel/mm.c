#include "mm.h"
#include "multiboot.h"
#include "kprint.h"
#include "powerctl.h"
#define MAXMEMINT 128
#define MAXMEMSTR "128"

extern uint8_t _kernel_end;   // defined by linker script

typedef struct block_header {
    uint32_t size;
    uint8_t  free;
    struct block_header* next;
} block_header_t;

static block_header_t* heap_head = 0;
static uint8_t* heap_end = 0;

static uint32_t align_up(uint32_t val, uint32_t align) {
    return (val + align - 1) & ~(align - 1);
}

void mm_init(uint32_t mb_info_addr) {
    struct multiboot_info* mb = (struct multiboot_info*)mb_info_addr;

    uint32_t heap_start = align_up((uint32_t)&_kernel_end, 0x1000);
    uint32_t heap_limit = heap_start + 0x00100000; // 1 MiB heap for now

    // Optionally: scan memory map and clamp heap_limit to usable RAM
    if (mb->flags & (1 << 6)) {
        uint32_t mmap_end = mb->mmap_addr + mb->mmap_length;
        kprint("Memory map:\n");
        for (uint32_t p = mb->mmap_addr; p < mmap_end; ) {
            struct multiboot_mmap_entry* e = (struct multiboot_mmap_entry*)p;
            // type 1 = available
            if (e->type == 1) {
                // You could use this to choose a better heap region later
            }
            p += e->size + sizeof(e->size);
        }
    }

    heap_head = (block_header_t*)heap_start;
    heap_head->size = heap_limit - heap_start - sizeof(block_header_t);
    heap_head->free = 1;
    heap_head->next = 0;

    heap_end = (uint8_t*)heap_limit;

    kprint("Heap initialized.\n");
}

static void split_block(block_header_t* block, uint32_t size) {
    if (block->size <= size + sizeof(block_header_t))
        return;

    uint8_t* new_addr = (uint8_t*)block + sizeof(block_header_t) + size;
    block_header_t* new_block = (block_header_t*)new_addr;

    new_block->size = block->size - size - sizeof(block_header_t);
    new_block->free = 1;
    new_block->next = block->next;

    block->size = size;
    block->next = new_block;
}

void* kmalloc(uint32_t size) {
    if (size == 0) return 0;

    size = align_up(size, 8);

    block_header_t* cur = heap_head;
    while (cur) {
        if (cur->free && cur->size >= size) {
            split_block(cur, size);
            cur->free = 0;
            return (uint8_t*)cur + sizeof(block_header_t);
        }
        cur = cur->next;
    }

    return 0; // out of memory
}

static void coalesce() {
    block_header_t* cur = heap_head;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            cur->size += sizeof(block_header_t) + cur->next->size;
            cur->next = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}

void kfree(void* ptr) {
    if (!ptr) return;

    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    block->free = 1;
    coalesce();
}

void* safemalloc(uint32_t safesize) {
    if (safesize > MAXMEMINT) {
        oom();
    }
    buffer = kmalloc(safesize);
    if (!buffer) {
        oom();
    }
    return buffer
    
}

static void oom() {
    kprint("OUT OF MEMORY. MAX MEMORY =" MAXMEMSTR);
    __asm__ volatile("int $34");
}
