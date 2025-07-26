#ifndef _XIEU_GDT_H
#define _XIEU_GDT_H

#include "types.h"

typedef union 
{
  u64 val;
  struct 
  {
    u64 limit_low : 16;
    u64 base_low : 16;
    u64 base_mid : 8;
    u64 segment_type : 4;
    u64 descriptor_type : 1;
    u64 dpl : 2;
    u64 present : 1;
    u64 limit_high : 4;
    u64 avl : 1;
    u64 long_mode : 1;
    u64 db : 1;
    u64 granularity : 1;
    u64 base_high : 8;
  };
} descriptor32_t;


typedef struct [[gnu::packed]]
{
  u16 size;
  descriptor32_t *gdt;
} gdtr_t;

[[gnu::aligned(8)]] descriptor32_t gdt[3] = {0};

gdtr_t gdtr = { .size = sizeof(gdt) - 1, .gdt = gdt };

void ia32e_gdt_load_flat()
{
  _Static_assert(sizeof(descriptor32_t) == 8, "descriptor32_t is 8 bytes");
  _Static_assert(sizeof(gdtr_t) == 10, "gdtr_t is 10 bytes");

  gdt[1] = ({
    descriptor32_t code = {0};
    code.long_mode = 1;
    code.present = 1;
    code.granularity = 1;
    code.segment_type = 0xA;
    code.descriptor_type = 1;
    code.dpl = 0;
    code.db = 0;
    code;
  });

  gdt[2] = ({
    descriptor32_t data = {0};
    data.present = 1;
    data.granularity = 1;
    data.segment_type = 0x2;
    data.descriptor_type = 1;
    data.dpl = 0;
    data;
  });

  asm volatile ("lgdt (%0)" :: "r" (&gdtr) : "memory");
  asm volatile (
    "mov $0x10, %%ax;"
    "mov %%ax, %%ds;"
    "mov %%ax, %%es;"
    "mov %%ax, %%fs;"
    "mov %%ax, %%gs;"
    "mov %%ax, %%ss;"
    "mov $0x8, %%ax;"
    "mov %%ax, %%cs;"
    ::: "ax", "memory"
  );
}

#endif
