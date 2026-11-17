#include "acpi.h"
#include "types.h"

/*
The Root System Description Pointer (RSDP) is the starting point for discovering ACPI tables. It is located in a specific region of memory, which can be searched to find the RSDP structure.

Because OS2 currently boots using GRUB Legacy and Multiboot 1, we can locate it through the conventional PC BIOS memory regions

The ACPI specification identifies two locations to search:
- The first 1 KiB of the Extended BIOS Data Area (EBDA)
- Physical addresses 0x000E0000 through 0x000FFFFF
*/

static const acpi_rsdp_t *acpi_rsdp = 0;
static const acpi_sdt_header_t *acpi_root = 0;
static const acpi_sdt_header_t *acpi_fadt = 0;
static bool acpi_initialized = false;

static bool acpi_checksum_valid(
    const void *address,
    u32int length
) {
    const u8int *bytes = (const u8int *)address;
    u8int sum = 0;

    for (u32int i = 0; i < length; i++) {
        sum = (u8int)(sum + bytes[i]);
    }

    return sum == 0;
}

static bool acpi_rsdp_signature_valid(
    const acpi_rsdp_t *rsdp
) {
    const char signature[8] = {
        'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '
    };

    for (u32int i = 0; i < 8; i++) {
        if (rsdp->signature[i] != signature[i])
            return false;
    }

    return true;
}

static const acpi_rsdp_t *acpi_scan_region(
    u32int start,
    u32int end
) {
    // Both addresses are exclusive/starting boundaries.
    // The scan must not read beyond end.

    for (u32int address = start;
         address <= end - sizeof(acpi_rsdp_t);
         address += 16) {

        const acpi_rsdp_t *rsdp =
            (const acpi_rsdp_t *)address;

        if (!acpi_rsdp_signature_valid(rsdp))
            continue;

        if (!acpi_checksum_valid(rsdp, 20))
            continue;

        if (rsdp->revision >= 2) {
            const acpi_xsdp_t *xsdp =
                (const acpi_xsdp_t *)address;

            // Validate that the extended structure fits
            // and its declared length is reasonable.
            if (xsdp->length < sizeof(acpi_xsdp_t))
                continue;

            if (xsdp->length > 4096)
                continue;

            if (xsdp->length > end - address)
                continue;

            if (!acpi_checksum_valid(xsdp, xsdp->length))
                continue;
        }

        return rsdp;
    }

    return 0;
}

const acpi_rsdp_t *acpi_find_rsdp(void) {
    // EBDA segment pointer stored in BIOS Data Area.
    u16int ebda_segment =
        *(volatile const u16int *)0x40E;

    u32int ebda_address =
        (u32int)ebda_segment << 4;

    // Search the first 1 KiB of EBDA.
    if (ebda_address >= 0x400 &&
        ebda_address <= 0x000FFC00) {

        const acpi_rsdp_t *rsdp =
            acpi_scan_region(
                ebda_address,
                ebda_address + 1024
            );

        if (rsdp)
            return rsdp;
    }

    // Search BIOS ROM address space.
    return acpi_scan_region(
        0x000E0000,
        0x00100000
    );
}


#include "acpi.h"
#include "tui.h"

bool acpi_init(void) {
    // Reset ACPI state
    acpi_initialized = false;
    acpi_rsdp = 0;
    acpi_root = 0;
    acpi_fadt = 0;

    // STEP 1: Locate the RSDP
    acpi_rsdp = acpi_find_rsdp();

    if (acpi_rsdp == 0) {
        tui_write("[ACPI] RSDP not found\n");
        return false;
    }

    tui_write("[ACPI] RSDP found\n");

    // STEP 2: Get the RSDT or XSDT address
    // TODO:
    // - Check the RSDP revision.
    // - Prefer XSDT if available.
    // - Otherwise use RSDT.
    // - Reject addresses that cannot be safely mapped.

    acpi_find_rsdp()

    // STEP 3: Validate the root table
    // TODO:
    // - Verify signature (RSDT/XSDT).
    // - Validate its length.
    // - Verify its checksum.

    // STEP 4: Locate the FADT
    // TODO:
    // - Iterate over addresses in the root table.
    // - Look for the "FACP" signature.
    // - Validate the FADT.
    // - Store its pointer in acpi_fadt.

    // STEP 5: Mark ACPI initialisation successful
    // TODO: Only do this when all steps succeeded.
    // acpi_initialized = true;

    return false; // Temporary until implemented
}


bool acpi_is_available(void){}
bool acpi_poweroff(void){}