# 1. Toolchain

NASM = nasm
LD = ld
CC = gcc
GENISOIMAGE = genisoimage

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -Wall -Wextra -Werror -c -I$(DRIVER_DIR) -I$(FRAMEBUFFER_DIR) -I$(TUI_DIR)

# 2. Directory Configuration

SRC_DIR = src
DRIVER_DIR = drivers
ISO_DIR = iso
FRAMEBUFFER_DIR = $(SRC_DIR)/framebuffer
TUI_DIR = $(SRC_DIR)/tui
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

# 3. Source Files and Build Outputs

ASM_SRC = $(SRC_DIR)/loader.asm
C_SRC = $(SRC_DIR)/kmain.c
IO_SRC = $(DRIVER_DIR)/io.c
TUI_SRC = $(TUI_DIR)/tui.c
FB_SRC = $(FRAMEBUFFER_DIR)/fb.c
LINKER = $(SRC_DIR)/link.ld

ASM_OBJ = $(SRC_DIR)/loader.o
C_OBJ = $(SRC_DIR)/kmain.o
IO_OBJ = $(DRIVER_DIR)/io.o
TUI_OBJ = $(TUI_DIR)/tui.o
FB_OBJ = $(FRAMEBUFFER_DIR)/fb.o

OBJECTS = $(ASM_OBJ) $(C_OBJ) $(IO_OBJ) $(TUI_OBJ) $(FB_OBJ)

KERNEL = $(BOOT_DIR)/kernel.elf
ISO = os.iso

# GRUB boot image path, relative to the project root

BOOTIMAGE = boot/grub/stage2_eltorito

# 4. Default Target

.PHONY: all
all: $(ISO)

# 5. Assemble the Bootloader

$(ASM_OBJ): $(ASM_SRC)
	$(NASM) -f elf $< -o $@

# 6. Compile the Kernel C Source

$(C_OBJ): $(C_SRC)
	$(CC) $(CFLAGS) $< -o $@

# 7. Compile the drivers and other C files

$(IO_OBJ): $(IO_SRC) $(DRIVER_DIR)/io.h
	$(CC) $(CFLAGS) -I$(DRIVER_DIR) $(IO_SRC) -o $@

$(TUI_OBJ): $(TUI_SRC) $(TUI_DIR)/tui.h
	$(CC) $(CFLAGS) -I$(TUI_DIR) $(TUI_SRC) -o $(TUI_DIR)/tui.o

$(FB_OBJ): $(FB_SRC) $(FRAMEBUFFER_DIR)/fb.h
	$(CC) $(CFLAGS) -I$(FRAMEBUFFER_DIR) $(FB_SRC) -o $(FRAMEBUFFER_DIR)/fb.o

# 8. Link the Kernel

$(KERNEL): $(OBJECTS) $(LINKER)
	mkdir -p $(BOOT_DIR)
	$(LD) -T $(LINKER) -m elf_i386 $(OBJECTS) -o $@

# 9. Build the Bootable ISO

$(ISO): $(KERNEL) $(GRUB_DIR)/stage2_eltorito
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
	rm -f $(OBJECTS) $(KERNEL) $(ISO)

# 11. Full Rebuild

.PHONY: rebuild
rebuild: clean all

# 12. Individual Build Targets

.PHONY: kernel
kernel: $(KERNEL)

.PHONY: iso
iso: $(ISO)

# 13. Help

.PHONY: help
help:
	@echo "OS2 Build System"
	@echo "----------------"
	@echo "make         - Build the ISO"
	@echo "make kernel  - Build kernel.elf"
	@echo "make iso     - Build os.iso"
	@echo "make clean   - Remove generated files"
	@echo "make rebuild - Clean and rebuild everything"
	@echo "make help    - Show available targets"
	@echo "make run	    - Runs .iso im QEMU environment"


# 14. Run

.PHONY: run

run: $(ISO)
	qemu-system-i386 \
	-monitor telnet:127.0.0.1:88888,server,nowait \
	-serial mon:stdio \
	-boot d \
	-cdrom $(ISO) \
	-m 32 \
	-display curses