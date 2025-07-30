#ifndef _XIEU_H
#define _XIEU_H

#include <stdint.h>
#include <stddef.h>
#include "types.h"

#include "gdt.h"

void* memcpy(void* dest, const void* src, size_t n)
{
  void* ret = dest;
  __asm__ volatile (
    "rep movsb"
    : "+D" (dest), "+S" (src), "+c" (n)
    :
    : "memory"
  );
  return ret;
}

void* memset(void* dest, int c, size_t n)
{
  void* ret = dest;
 __asm__ volatile (
    "rep stosb"
    : "+D" (dest), "+c" (n)
    : "a" (c)
    : "memory"
  );
  return ret;
}

void* memmove(void* dest, const void* src, size_t n)
{
  void* ret = dest;

  // Check if we need to copy backwards to avoid overlap issues
  if (dest > src && (char*)dest < (char*)src + n)
  {
    // Copy backwards
    dest = (char*)dest + n - 1;
    src = (char*)src + n - 1;
    __asm__ volatile (
      "std\n\t"        // Set direction flag (backwards)
      "rep movsb\n\t"
      "cld"            // Clear direction flag back to normal
      : "+D" (dest), "+S" (src), "+c" (n)
      :
      : "memory"
    );
  } else {
    // Copy forwards (same as memcpy)
    __asm__ volatile (
      "rep movsb"
      : "+D" (dest), "+S" (src), "+c" (n)
      :
      : "memory"
    );
  }
  return ret;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  int result;
  __asm__ volatile (
    "repe cmpsb\n\t"
    "seta %%al\n\t"      // Set AL if above (s1 > s2)
    "setb %%dl\n\t"      // Set DL if below (s1 < s2)
    "subb %%dl, %%al\n\t" // AL = AL - DL, gives -1, 0, or 1
    "movsbw %%al, %%ax"   // Sign extend to 16-bit
    : "=a" (result), "+S" (s1), "+D" (s2), "+c" (n)
    :
    : "memory", "edx"
  );
  return result;
}

static void hcf(void) {
  for (;;) asm ("hlt");
}

#endif
