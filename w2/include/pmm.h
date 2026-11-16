#ifndef PMM_H
#define PMM_H

#include "multiboot.h"
#include "types.h"

void pmm_init(multiboot_info_t *mb_info);
void uint64_to_hex(u64int value, char *buffer);

#endif