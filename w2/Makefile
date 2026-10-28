# 1. Toolchain
NASM = nasm
LD = ld
CC = gcc
GENISOIMAGE = genisoimage
QEMU = qemu-system-i386


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


# 3. Compiler Flags
CFLAGS = -m32 -ffreestanding -fno-pie \
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


C_OBJECTS = $(C_OBJ) $(IO_OBJ) $(TUI_OBJ) $(FB_OBJ) $(GDT_C_OBJ) $(KB_OBJ) $(KSTRING_OBJ) \
            $(PIC_OBJ) $(INTERRUPTS_C_OBJ)
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
	$(NASM) -f elf32 $< -o $@

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
.PHONY: run
run: $(ISO)
	$(QEMU) \
		-monitor telnet:127.0.0.1:8888,server,nowait \
		-serial null \
		-boot d \
		-cdrom $(ISO) \
		-m 32 \
		-display curses


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
