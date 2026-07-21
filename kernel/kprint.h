#pragma once
#include <stdarg.h>
#include <stdint.h>

void kclear(void);
void kputc(char c);
void kprint(const char* s);
void kprintf(const char* fmt, ...);
void change_color(uint8_t color);
