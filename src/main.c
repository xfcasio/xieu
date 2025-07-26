#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limine.h>
#include "main.h"

[[gnu::used, gnu::section(".limine_requests")]]
static volatile LIMINE_BASE_REVISION(3);

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

void kmain(void)
{
  if (!LIMINE_BASE_REVISION_SUPPORTED) hcf();

  if (!framebuffer_request.response || framebuffer_request.response->framebuffer_count < 1) hcf();

  ia32e_gdt_load_flat();

  struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
  volatile uint32_t *fb_ptr = framebuffer->address;
  for (size_t i = 0; i < 200; i++)
    fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;

  hcf();
}

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

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
