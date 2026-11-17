#ifndef POWER_H
#define POWER_H

#include "types.h"

typedef struct {
    char signature[8];
    u8int checksum;
    char oem_id[6];
    u8int revision;
    u32int rsdt_address;
} __attribute__((packed)) acpi_rsdp_t;

typedef struct {
    acpi_rsdp_t first;
    u32int length;
    u64int xsdt_address;
    u8int extended_checksum;
    u8int reserved[3];
} __attribute__((packed)) acpi_xsdp_t;

const acpi_rsdp_t *acpi_find_rsdp(void);
void system_shutdown_qemu(void);

#endif // POWER_H