#ifndef ACPI_H
#define ACPI_H
    
#include "types.h"
#include "kbool.h"

typedef struct {
    char signature[4];
    u32int length;
    u8int revision;
    u8int checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32int oem_revision;
    u32int creator_id;
    u32int creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

bool acpi_init(void);
bool acpi_is_available(void);
bool acpi_poweroff(void);

#endif // ACPI_H