
# 1. Cross-platform toolchain configuration

UNAME_S := $(shell uname -s)

NASM := nasm
QEMU := qemu-system-i386
FB_MODE ?= 0

NASMFLAGS := -DFB_MODE=$(FB_MODE)
ifeq ($(UNAME_S),Darwin)
    # macOS - Apple Silicon or Intel
    CC := i686-elf-gcc
    LD := i686-elf-ld
    GENISOIMAGE := mkisofs
    QEMU_DISPLAY := cocoa,zoom-to-fit=on
    QEMU_ACCEL := tcg
	QEMU_VGA := std
    QEMU_SERIAL := stdio

else ifeq ($(UNAME_S),Linux)
    # Linux, including Windows Subsystem for Linux
    CC := gcc
    LD := ld
    GENISOIMAGE := genisoimage
    QEMU_DISPLAY := curses
    QEMU_ACCEL := tcg
	QEMU_VGA := std
    QEMU_SERIAL := null
    QEMU_SERIAL := null

else
    $(error Unsupported build environment: $(UNAME_S). Use macOS, Linux, or WSL on Windows)
endif



# 2. Directory Configuration
SRC_DIR = src
DRIVER_DIR = drivers
INCLUDE_DIR = include
ISO_DIR = iso
FRAMEBUFFER_DIR = $(SRC_DIR)/framebuffer
TUI_DIR = $(SRC_DIR)/tui
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub
GDT_DIR = $(SRC_DIR)/gdt
KEYBOARD_DIR = $(SRC_DIR)/keyboard
KSTRING_DIR = $(DRIVER_DIR)
PIC_DIR = $(SRC_DIR)/pic
INTERRUPTS_DIR = $(SRC_DIR)/interrupts
INPUT_DIR = $(SRC_DIR)/input
SHELL_DIR = $(SRC_DIR)/shell
TIMER_DIR = $(SRC_DIR)/timer
MEM_DIR = $(SRC_DIR)/memory
POWER_DIR = $(SRC_DIR)/power
ACPI_DIR = $(SRC_DIR)/acpi
PIT_DIR = $(SRC_DIR)/pit


# 3. Compiler Flags
CFLAGS = -m32 -ffreestanding -fno-pie -DFB_MODE=$(FB_MODE) \
         -fno-stack-protector -Wall -Wextra -Werror \
         -I$(INCLUDE_DIR) -MMD -MP


# 4. Source Files and Build Outputs
ASM_SRC = $(SRC_DIR)/loader.asm
C_SRC = $(SRC_DIR)/kmain.c
IO_SRC = $(DRIVER_DIR)/io.c
TUI_SRC = $(TUI_DIR)/tui.c
GDT_C_SRC = $(GDT_DIR)/gdt.c
GDT_ASM_SRC = $(GDT_DIR)/gdt.asm
FB_SRC = $(FRAMEBUFFER_DIR)/fb.c
KB_SRC = $(KEYBOARD_DIR)/keyboard.c
KSTRING_SRC = $(KSTRING_DIR)/kstring.c
PIC_SRC = $(PIC_DIR)/pic.c
INTERRUPTS_C_SRC = $(INTERRUPTS_DIR)/interrupts.c
INTERRUPTS_ASM_SRC = $(INTERRUPTS_DIR)/interrupts.asm
IDT_ASM_SRC = $(INTERRUPTS_DIR)/idt.asm
INPUT_SRC = $(INPUT_DIR)/input.c
SHELL_SRC = $(SHELL_DIR)/shell.c
TIMER_SRC = $(TIMER_DIR)/timer.c
PMM_SRC = $(MEM_DIR)/pmm.c
PAGING_SRC = $(MEM_DIR)/paging.c
POWER_SRC = $(POWER_DIR)/power.c
# ACPI_SRC = $(ACPI_DIR)/acpi.c
# TABLES_SRC = $(ACPI_DIR)/tables.c
PIT_SRC = $(PIT_DIR)/pit.c
HEAP_SRC = $(MEM_DIR)/heap.c


LINKER = $(SRC_DIR)/link.ld


ASM_OBJ = $(SRC_DIR)/loader.o
C_OBJ = $(SRC_DIR)/kmain.o
IO_OBJ = $(DRIVER_DIR)/io.o
TUI_OBJ = $(TUI_DIR)/tui.o
FB_OBJ = $(FRAMEBUFFER_DIR)/fb.o
GDT_C_OBJ = $(GDT_DIR)/gdt.o
GDT_ASM_OBJ = $(GDT_DIR)/gdt_asm.o
KB_OBJ = $(KEYBOARD_DIR)/keyboard.o
KSTRING_OBJ = $(KSTRING_DIR)/kstring.o
PIC_OBJ = $(PIC_DIR)/pic.o
INTERRUPTS_C_OBJ = $(INTERRUPTS_DIR)/interrupts.o
INTERRUPTS_ASM_OBJ = $(INTERRUPTS_DIR)/interrupts_asm.o
IDT_ASM_OBJ = $(INTERRUPTS_DIR)/idt_asm.o
INPUT_OBJ = $(INPUT_DIR)/input.o
SHELL_OBJ = $(SHELL_DIR)/shell.o
TIMER_OBJ = $(TIMER_DIR)/timer.o
PMM_OBJ = $(MEM_DIR)/pmm.o
PAGING_OBJ = $(MEM_DIR)/paging.o
POWER_OBJ = $(POWER_DIR)/power.o
# ACPI_OBJ = $(ACPI_DIR)/acpi.o
# TABLES_OBJ = $(ACPI_DIR)/tables.o
PIT_OBJ = $(PIT_DIR)/pit.o
HEAP_OBJ = $(MEM_DIR)/heap.o

C_OBJECTS = $(C_OBJ) $(IO_OBJ) $(TUI_OBJ) $(FB_OBJ) $(GDT_C_OBJ) $(KB_OBJ) $(KSTRING_OBJ) \
            $(PIC_OBJ) $(INTERRUPTS_C_OBJ) $(INPUT_OBJ) $(SHELL_OBJ) $(TIMER_OBJ) $(PMM_OBJ) $(PAGING_OBJ) \
			$(POWER_OBJ) $(PIT_OBJ) $(HEAP_OBJ) # $(ACPI_OBJ) $(TABLES_OBJ)

OBJECTS = $(ASM_OBJ) $(GDT_ASM_OBJ) $(INTERRUPTS_ASM_OBJ) $(IDT_ASM_OBJ) $(C_OBJECTS)
DEPS = $(C_OBJECTS:.o=.d)


KERNEL = $(BOOT_DIR)/kernel.elf
ISO = os.iso
BOOTIMAGE = boot/grub/stage2_eltorito


# 5. Default Target
.PHONY: all
all: $(ISO)


# 6. Assemble Bootloader, GDT and Interrupt Support
$(ASM_OBJ): $(ASM_SRC)
	$(NASM) $(NASMFLAGS) -f elf32 $< -o $@

$(GDT_ASM_OBJ): $(GDT_ASM_SRC)
	$(NASM) -f elf32 $< -o $@

$(INTERRUPTS_ASM_OBJ): $(INTERRUPTS_ASM_SRC)
	$(NASM) -f elf32 $< -o $@

$(IDT_ASM_OBJ): $(IDT_ASM_SRC)
	$(NASM) -f elf32 $< -o $@


# 7. Compile C Source Files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# 8. Link Kernel
$(KERNEL): $(OBJECTS) $(LINKER)
	mkdir -p $(BOOT_DIR)
	$(LD) -T $(LINKER) -m elf_i386 $(OBJECTS) -o $@


# 9. Build Bootable ISO
$(ISO): $(KERNEL) $(GRUB_DIR)/stage2_eltorito $(GRUB_DIR)/menu.lst
	$(GENISOIMAGE) -R \
		-b $(BOOTIMAGE) \
		-no-emul-boot \
		-boot-load-size 4 \
		-A os \
		-input-charset utf8 \
		-quiet \
		-boot-info-table \
		-o $@ \
		$(ISO_DIR)


# 10. Clean Build Artifacts
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(DEPS) $(KERNEL) $(ISO)


# 11. Full Rebuild
.PHONY: rebuild
rebuild: clean
	$(MAKE) all


# 12. Individual Build Targets
.PHONY: kernel iso
kernel: $(KERNEL)
iso: $(ISO)



# 13. Run in QEMU

.PHONY: run run_testmode

run: $(ISO)
	$(QEMU) \
		-monitor telnet:127.0.0.1:45454,server,nowait \
		-serial $(QEMU_SERIAL) \
		-boot d \
		-cdrom $(ISO) \
		-m 32 \
		-vga $(QEMU_VGA) \
		-display $(QEMU_DISPLAY) \
		-accel $(QEMU_ACCEL) \

run_testmode: $(ISO)
	$(QEMU) \
		-serial $(QEMU_SERIAL) \
		-boot d \
		-cdrom $(ISO) \
		-m 32 \
		-no-reboot \
		-vga $(QEMU_VGA) \
		-display $(QEMU_DISPLAY) \
		-accel $(QEMU_ACCEL) \
		-display $(QEMU_DISPLAY) \
		-d int \
		-D qemu-interrupts.log


# 14. Help
.PHONY: help
help:
	@echo "OS2 Build System"
	@echo "----------------"
	@echo "make         - Build the ISO"
	@echo "make kernel  - Build kernel.elf"
	@echo "make iso     - Build os.iso"
	@echo "make clean   - Remove generated files"
	@echo "make rebuild - Clean and rebuild everything"
	@echo "make run     - Run ISO in QEMU"
	@echo "make help    - Show available targets"
	

# 15. Automatically Track Header Dependencies
-include $(DEPS)
