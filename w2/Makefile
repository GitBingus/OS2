SRC_DIR = src
ISO_DIR = iso
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub
# 1. Toolchain

NASM = nasm
LD = ld
CC = gcc
GENISOIMAGE = genisoimage

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -Wall -Wextra -c

# 2. Directory Configuration

SRCDIR = src 
ISODIR = iso 
BOOTDIR = $(ISO_DIR)/boot 
GRUBDIR = $(BOOT_DIR)/grub

# Source Files and Build Outputs

ASM_SRC = $(SRC_DIR)/loader.asm
C_SRC = $(SRC_DIR)/kmain.c
LINKER = $(SRC_DIR)/link.ld

ASM_OBJ = $(SRC_DIR)/loader.o
C_OBJ = $(SRC_DIR)/kmain.o
OBJECTS = $(ASM_OBJ) $(C_OBJ)

KERNEL = $(BOOT_DIR)/kernel.elf
ISO = os.iso

# GRUB boot image path, relative to iso/

BOOTIMAGE = boot/grub/stage2_eltorito

# 4. Default Target

.PHONY: all 
all: $(ISO)

# 5. Assemble the Bootloader

$(ASM_OBJ): $(ASM_SRC) 
	$(NASM) -f elf $< -o $@

# 5.5. Link C

$(C_OBJ): $(C_SRC)
	$(CC) $(CFLAGS) $< -o $@

# 6. Link the Kernel

$(KERNEL): $(OBJECTS) $(LINKER)
	$(LD) -T $(LINKER) -m elf_i386 $(OBJECTS) -o $@

# 7. Build the Bootable ISO

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

# 8. Clean Build Artifacts

.PHONY: clean 
clean: 
	rm -f $(OBJ) $(KERNEL) $(ISO)

# 9. Full Rebuild

.PHONY: rebuild 
rebuild: clean all

# 10. Individual Build Targets

.PHONY: kernel 
kernel: $(KERNEL)

# Build the ISO.

.PHONY: iso 
iso: $(ISO)

# 11. Help

.PHONY: help 
help: @echo "OS2 Build System" @echo "----------------" @echo "make - Build the ISO" @echo "make kernel - Build kernel.elf" @echo "make iso - Build os.iso" @echo "make clean - Remove generated files" @echo "make rebuild - Clean and rebuild everything" @echo "make help - Show available targets"