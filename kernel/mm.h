#pragma once
#include <stdint.h>

void mm_init(uint32_t mb_info_addr);
void* kmalloc(uint32_t size);
void* safemalloc(uint32_t safesize);
void kfree(void* ptr);
