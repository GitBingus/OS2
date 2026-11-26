#ifndef PMM_H
#define PMM_H

#include "multiboot.h"
#include "types.h"
#include "kbool.h"

#define PMM_FRAME_SIZE 4096

void pmm_init(multiboot_info_t *mb_info);
void uint64_to_hex(u64int value, char *buffer);

u32int pmm_get_free_frames(void);
u32int pmm_alloc_frame(void);
u32int pmm_alloc_frames(u32int count);
u32int pmm_alloc_dma_frames(u32int count);
bool pmm_free_frame(u32int address);
bool pmm_free_dma_frames(u32int address, u32int count);
void pmm_mark_region_used(u64int base, u64int length);

#endif