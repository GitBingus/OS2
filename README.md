# OS2 — 32-bit Operating System

A small 32-bit x86 operating system developed in C and NASM as part of the Operating Systems 2 module at the University of the West of England (UWE).

OS2 is an educational operating system project focused on understanding the fundamentals of low-level programming, hardware interaction, memory management, graphics and kernel development.

The project is built from the ground up, using GRUB for bootloading and QEMU for emulation and testing.

---

## Features

### Kernel & Hardware

- **32-bit x86 architecture** — Freestanding C kernel with NASM assembly routines.
- **GRUB bootloader** — Multiboot-compatible kernel initialisation.
- **Global Descriptor Table (GDT)** — Kernel code and data segments.
- **Interrupt Descriptor Table (IDT)** — CPU exception and hardware interrupt handling.
- **Programmable Interrupt Controller (PIC)** — Hardware IRQ management.
- **Programmable Interval Timer (PIT)** — System timing and sleep functionality.
- **Keyboard driver** — PS/2 keyboard input and key event handling.
- **Port I/O** — Direct hardware communication through x86 I/O ports.

### Memory Management

- **Physical Memory Manager (PMM)** — Physical memory frame allocation.
- **Paging** — Page directory and page table management.
- **Virtual Memory** — Mapping and unmapping virtual memory pages.
- **Kernel Heap** — Dynamic memory allocation using `kmalloc()` and `kfree()`.

### Display & Graphics

OS2 includes multiple display approaches, with a display abstraction intended to provide a consistent interface across different rendering modes.

- **VGA Text Mode** — Character-based rendering using VGA text memory.
- **Graphical Framebuffer** — Direct pixel manipulation through a linear framebuffer.
- **32-bit RGB Colour** — Full RGB colour values rather than the limited VGA text palette.
- **Pixel Rendering** — Individual pixel drawing.
- **Rectangle Rendering** — Filled rectangular shapes.
- **Scrolling** — Framebuffer content scrolling.
- **Character Rendering** — Bitmap font rendering for graphical output.

- The original VGA Text Mode has been retained

### User Interface

- Text-based terminal interface.
- Keyboard-driven input.
- Character and colour management.
- Display mode abstraction.
- Command-line interaction.

### Games & Applications

**Tetris**

A Tetris implementation developed as an application running within OS2.

Planned and implemented game features include:

- Tetromino generation and rotation.
- Piece movement and collision detection.
- Gravity and piece locking.
- Line clearing and scoring.
- Graphical board rendering.
- Keyboard controls.
- Splash screen and game interface.

### TO COME

- Sound
- Scoring
- Splash-screen and end-screen

---

## Screenshots & Development

---

## Building & Running

### Requirements

The following tools are required to build and run OS2:

| Tool | Purpose |
|---|---|
| GCC (32-bit support) | C compilation |
| NASM | Assembly compilation |
| GNU LD | Kernel linking |
| genisoimage | Bootable ISO generation |
| QEMU | Operating system emulation |
| Make | Build automation |

### Linux / WSL

Install the required packages:

```bash
sudo apt update
sudo apt install build-essential gcc-multilib nasm genisoimage qemu-system-x86
```

### macOS

Using Homebrew:

```bash
brew install nasm gcc qemu cdrtools
```

Ensure that the appropriate 32-bit x86 toolchain and linker configuration are available.

### Compiling

Navigate to the kernel directory:

```bash
cd w2
```

Build the operating system:

```bash
make
```

OR 

```bash
make rebuild
```

This generates the bootable `os.iso` image.

### Running

Launch OS2 using QEMU:

```bash
make run
```

Alternatively, boot the ISO directly:

```bash
qemu-system-i386 -cdrom os.iso -boot d -m 32
```

### Make Commands

| Command | Description |
|---|---|
| `make` | Build the bootable ISO |
| `make kernel` | Build the kernel ELF |
| `make iso` | Generate the bootable ISO |
| `make run` | Launch OS2 in QEMU |
| `make run_testmode` | Launch QEMU in test mode |
| `make clean` | Remove generated build files |
| `make rebuild` | Clean and rebuild the project |
| `make help` | Display available Make commands |

---

## Project Structure

The repository is organised into three main development stages:

```text
OS2/
│
├── w0/                 # Worksheet 0
│
├── w1/                 # Worksheet 1
│
└── w2/                 # Main operating system
    │
    ├── drivers/        # Hardware drivers and utilities
    │
    ├── include/        # Shared header files
    │
    ├── iso/            # Bootable ISO configuration
    │
    ├── src/
    │   │
    │   ├── framebuffer/ # VGA and graphical framebuffer
    │   ├── tui/         # Terminal user interface
    │   ├── gdt/         # Global Descriptor Table
    │   ├── interrupts/  # Interrupt handling
    │   ├── keyboard/    # Keyboard driver
    │   ├── memory/      # Memory management
    │   ├── pic/         # Programmable Interrupt Controller
    │   ├── pit/         # Programmable Interval Timer
    │   ├── input/       # Input handling
    │   ├── shell/       # Command-line interface
    │   │
    │   ├── loader.asm   # Kernel entry point
    │   ├── kmain.c      # Kernel initialisation
    │   └── link.ld      # Linker script
    │
    ├── Makefile
    └── os.iso
```

*The directory structure may evolve as development continues.*

---

## Development Goals

OS2 is an ongoing project. The broader aim is to develop an increasingly capable operating system while learning how its individual components interact.

Areas of continued development include:

- Completing and refining the graphical display subsystem.
- Improving font rendering and graphical terminal support.
- Expanding hardware and device driver support.
- Improving memory management and allocation.
- Developing additional applications and games.
- Expanding kernel functionality and system architecture.

In the long distant future, i would like to add:
- Mouse support
- A full GUI
- Hardware acceleration
- File systems
- **MAYBE** upgrade to 64-bit mode

---

## Technologies

- **C** — Kernel and system development.
- **NASM** — x86 assembly.
- **GCC** — Compilation.
- **GNU LD** — Linking.
- **GRUB** — Bootloading.
- **QEMU** — Emulation and testing.
- **Make** — Build automation.
- **Git** — Version control.

---

## References

- [The Little Book About OS Development](https://littleosbook.github.io/)
- [OSDev Wiki](https://wiki.osdev.org/)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [Intel 64 and IA-32 Architectures Software Developer's Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [QEMU Documentation](https://www.qemu.org/docs/master/)
- [Sortix](https://sortix.org/)
- [Many Wikipedia pages, but mainly](https://en.wikipedia.org/wiki/X86#32-bit)

---

**OS2 — University of the West of England**

