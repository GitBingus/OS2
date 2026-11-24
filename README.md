# OS2

A small 32-bit x86 operating system kernel written in C and NASM, booted by GRUB 2 (Multiboot 1) and run in QEMU. It was built for the Operating Systems 2 module at <a name="[UWE SCCT](https://github.com/uwe-scct)"></a>UWE and ends in an interactive shell with keyboard input, a timer, memory management and a set of self-test commands.

| Directory | Contents |
|-----------|----------|
| `w0/` | Worksheet 0: toolchain check (C++ hello world) |
| `w1/` | Worksheet 1: NASM exercises with a C driver, with screenshots |
| `w2/` | Worksheet 2 onwards: the OS2 kernel (everything below) |

## Features

- **Boot:** Multiboot 1 header in `loader.asm`, stack setup, then `kmain()` with the Multiboot info pointer.
- **CPU setup:** flat 3-entry GDT (null, kernel code, kernel data); PIC remapped; IDT with all 32 CPU exceptions reporting a diagnostic, plus IRQ0 and IRQ1.
- **Display:** 80x25 VGA text mode with colour attributes, hardware cursor, newline, backspace and scrolling.
- **Keyboard:** PS/2 Set 1 decoding (Shift, Caps Lock, arrows), IRQ1 handler feeding an event queue, `getc()` and a bounded `readline()` with in-line editing and command history.
- **Timer:** PIT at 100 Hz giving tick count, uptime in ms and `pit_sleep_ms()`.
- **Memory:**
  - bitmap physical frame allocator seeded from the Multiboot memory map;
  - paging enabled, with 128 MiB identity-mapped and `paging_map()` / `paging_unmap()`;
  - `vm_alloc_pages()` / `vm_free_pages()` for virtual pages at `0x08000000`–`0x10000000`;
  - a 64 KiB first-fit kernel heap (`kmalloc()` / `kfree()`).
- **Shell:** command table with `argc`/`argv` dispatch.

## Build and run

Work from `w2/`.

**macOS (Homebrew)**

```sh
brew install nasm i686-elf-gcc i686-elf-binutils i686-elf-grub xorriso qemu
```

The Makefile must use `MKRESCUE := i686-elf-grub-mkrescue` on macOS. Use the i686 GRUB, not `x86_64-elf-grub`, which produces an EFI-only ISO that QEMU's BIOS cannot boot.

**Linux / WSL**

```sh
sudo apt install build-essential gcc-multilib nasm grub-pc-bin xorriso mtools qemu-system-x86
```

**Make targets**

| Command | Action |
|---------|--------|
| `make` / `make iso` | Build `os.iso` |
| `make kernel` | Build only `iso/boot/kernel.elf` |
| `make run` | Boot the ISO in QEMU (32 MB RAM); the QEMU monitor listens on `telnet 127.0.0.1 45454` |
| `make run_testmode` | Boot with `-no-reboot` and log interrupts to `qemu-interrupts.log` |
| `make rebuild` | Clean, then build |
| `make clean` | Remove objects, kernel and ISO |

To boot by hand: `qemu-system-i386 -cdrom os.iso -boot d -m 32`.

## Using the shell

At the `>` prompt, type a command and press Enter. Left/Right move the cursor, Backspace edits at the cursor, and Up/Down recall the last 16 commands. `help` lists commands and `help <command>` describes one.

| Command | Description |
|---------|-------------|
| `help [command]` | List commands or describe one |
| `clear` | Clear the screen |
| `version` | Show the OS2 version |
| `echo [-n] text` | Print arguments (`-n` omits the newline) |
| `meminfo` | Show memory totals |
| `memtest` | Physical frame allocator test |
| `pagetest` | Map, write, read and unmap a virtual page |
| `vmtest` | Virtual page allocator test, including invalid and double frees |
| `heap` | `kmalloc()` / `kfree()` test |
| `pittest` | Show PIT ticks and uptime |
| `sleep [-m\|-s\|-M\|-h] n` | Sleep for n ms (default), seconds, minutes or hours |
| `keytest` | Keyboard input test; enter `ESC` to exit |
| `div0` | Trigger a divide-by-zero exception |
| `shutdown` | Power off (QEMU only) |

## How it boots

```
GRUB 2 -> loader.asm (stack, push Multiboot info) -> kmain()
  GDT -> terminal -> PMM -> PIC + IDT -> paging -> heap -> PIT -> unmask IRQ0/1 -> sti -> shell
```

Keyboard IRQs queue events; `getc()` and the shell consume them outside the interrupt handler.

## Source layout (`w2/`)

```
src/        kmain.c, loader.asm, link.ld, and one folder per subsystem:
            gdt, interrupts, pic, pit, keyboard, input, framebuffer, tui,
            memory (pmm, paging, heap), shell, power, acpi
drivers/    io.c (port I/O), kstring.c (string helpers)
include/    headers
iso/boot/grub/   grub.cfg (copied into the ISO with the kernel)
```

## Limitations

- BIOS boot only: a 32-bit Multiboot 1 kernel that expects VGA text mode, so it will not boot under UEFI.
- Shell arguments are split on whitespace only, with no quoting or escapes.
- `meminfo` assumes 128 MiB total, so its totals are wrong with `make run`'s 32 MB.
- `shutdown` uses QEMU's power-off port (`0x604`), so it does nothing on real hardware.
- `src/acpi/` is a work in progress and is excluded from the build.
- No user mode, scheduler, file system or system calls yet.

## References

- Helin, E. and Renberg, A., *The little book about OS development* (2015) — https://littleosbook.github.io
- *Multiboot Specification* — https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
- OSDev Wiki — https://wiki.osdev.org (GDT, IDT, PIC, PIT, PS/2 keyboard, paging, VGA text mode)
- *Intel 64 and IA-32 Architectures Software Developer's Manual*
- QEMU documentation — https://www.qemu.org/docs/master/
