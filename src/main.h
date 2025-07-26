#ifndef _XIEU_H
#define _XIEU_H

#include <stdint.h>
#include <stddef.h>
#include "types.h"

static void hcf(void);

void* memcpy(void* dest, const void* src, usize n);
void* memset(void* dest, int c, usize n);
void* memmove(void* dest, const void* src, usize n);
int memcmp(const void* s1, const void* s2, usize n);

#include "gdt.h"

#endif
