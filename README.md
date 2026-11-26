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

Unfortunately the screenshots I took from my Macbook don't exist anymore, but my PC screenshots do exist
The screenshots posted here are what I deem to be the most relevant, but look in the `images/` folder for all of them!

![TUI with text](images/Screenshot%202026-10-31%20154225.png "First working instance of console")

This is the first instance my TUI working at all, before the shell or anything like that.

![Exception 0 Test](images/Screenshot%202026-10-31%20155811.png "Exception 0 Test")
![Exception 0 Test](images/Screenshot%202026-10-31%20161858.png "Exception 0 Test")

These 2 images are from the development of the interrupts and critical exceptions, this one specifically CPU EXCEPTION 0: Division by 0.

![Memory Testing](images/Screenshot%202026-11-16%20143956.png "Memory Testing")

This screenshot is from my long and painful endeavor into memory initialisation, paging and testing, including reserving memory for the kernel and crucial infrastructural code, like the GDT, IDT and PIC.

![Memory Testing](images/Screenshot%202026-11-16%20232920.png "Memory Testing")

This screenshot shows the time my paging and memory allocations all worked successfully. You can see it is working fine by the fact all of the correct ranges and lengths are either marked as reserved or available, based on what is needed to be reserved.

![PIT Testing](images/Screenshot%202026-11-17%20130214.png "PIT Testing")

Here is a screenshot of me validating my newly implemented PIT, or Programmable Interval Timer. This system is responsible for keeping track of CPU ticks, as well as for scheduling, sleeping and interrupts. Here we can see it working fine.

![Memory Testing](images/Screenshot%202026-11-19%20130500.png "Memory Testing")

Here we can see me validating my new VMM, or Virtual Memory Manager system using the new `vmtest` command in the shell. The VMM works in conjunction with the PMM, or Physical Memory Manager, to map fake virtual addresses to those physical frames so each program gets its own isolated, continuous address space.

![First go at GPU rendering](images/Screenshot%202026-11-20%20133844.png "First go at GPU rendering")

This was my first attempt at graphical rendering instead of using the VGA Text Mode. I grossly misunderstood what everything meant, and kept getting very weird and benign errors, until I scrapped the project entirely in favour of the VGA mode. At this point I wasn't going to bother making any games, leave alone Tetris, so i kept with the VGA mode

![First go at TETRIS](images/Screenshot%202026-11-24%20144514.png "First go at TETRIS")
![First go at TETRIS](images/Screenshot%202026-11-24%20144316.png "First go at TETRIS")
![First go at TETRIS](images/Screenshot%202026-11-24%20151718.png "First go at TETRIS")
![First go at TETRIS](images/Screenshot%202026-11-24%20152557.png "First go at TETRIS")
![First go at TETRIS](images/Screenshot%202026-11-24%20154558.png "First go at TETRIS")

Here are a lot of images from the inital development of my implementation of Tetris. At this point i was still using the VGA mode, and was using a very hacky workaround to display the tetrominoes, whereby I would draw the VGA character for a solid block to the screen, the failed attampts of which you can admire above.

At this point I was forced to rediscover graphics mode instead of VGA mode, and luckily I got it working! 
The issue was I was ignoring the flags generated by the multiboot information, so i was feeling completely benign data as graphical data, hence it not working. Alas.

![Second go at TETRIS](images/Screenshot%202026-11-25%20131033.png "Second go at TETRIS")

About a day later I had the framebuffer mode fully implemented, so I generated a mosaic using Python and transplanted them into C, the results of which are here! You can see I made the mosaic up out of the tetrominoes, but to make it look better I added a process called bevelling, where the bottom left to top right pixels, about a couple pixels deep, are shaded light, and the top right to bottom left pixels are shaded dark, making each block appear '3D'. The results of which are shown below.

![Second go at TETRIS](images/Screenshot%202026-11-25%20182337.png "Second go at TETRIS")

You can see each individual tetrominoe instead of just a solid block on colour now.

Finally, here are some images of funny, corrupted tetrominoes caused by uninitialised memory.

![Second go at TETRIS](images/Screenshot%202026-11-26%20123553.png "Second go at TETRIS")
![Second go at TETRIS](images/Screenshot%202026-11-26%20131738.png "Second go at TETRIS")
![Second go at TETRIS](images/Screenshot%202026-11-26%20132350.png "Second go at TETRIS")
![Second go at TETRIS](images/Screenshot%202026-11-26%20132430.png "Second go at TETRIS")

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

