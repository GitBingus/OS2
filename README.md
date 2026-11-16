# OS2 — 32-bit x86 Operating System

GD_OS is a small, freestanding 32-bit x86 operating-system kernel written in **C and NASM**, booted with **GRUB** and tested in **QEMU**. The `w2/` project covers the work for Operating Systems Worksheet 2 Parts 1 and 2, and is being extended with an interactive command-line shell.

## Features

### Kernel and hardware

- Freestanding 32-bit x86 kernel loaded through GRUB.
- Three-entry flat Global Descriptor Table (GDT): null, kernel code, and kernel data.
- Interrupt Descriptor Table (IDT) with CPU exception handlers and keyboard interrupt support.
- 8259 PIC remapping: master at vector `0x20`, slave at `0x28`.
- Keyboard IRQ1 handled at IDT vector `0x21` (33), with EOI acknowledgement.
- VGA text-mode framebuffer output, hardware cursor, screen clearing, and scrolling.
- Exception diagnostics, including a division-by-zero test.

### Keyboard and input

- Interrupt-driven PS/2 keyboard driver with a fixed-size event queue.
- US QWERTY Set 1 scancode mapping, including letters, digits, punctuation, Shift and Caps Lock.
- Enter, Backspace, Tab and arrow-key events decoded by the keyboard driver.
- `getc()` for character-oriented input and a special-key-aware input path for line-editing work.
- `readline()` with bounded, null-terminated input and Backspace handling.
- Raw scancode logging and counters used during keyboard troubleshooting.

**In progress:** Up/Down command-history recall; Left/Right editing, insertion and deletion within a line; handling wrapped or scrolling input lines.

### Shell

- Interactive prompt and command dispatch using a table of command names, descriptions and function pointers.
- Basic `argc`/`argv` tokenisation; the tokenizer modifies the writable input buffer.
- Built-in commands introduced during development:

  | Command | Purpose |
  | --- | --- |
  | `help` | Show commands and, where supported, help for a specific command |
  | `clear` | Clear the text screen |
  | `version` | Display the OS2 version |
  | `echo` | Print arguments; `-n` suppresses the trailing newline |
  | `keytest` | Exercise keyboard input |
  | `div0` | Trigger a deliberate divide-error exception for testing |

> **Caution:** `div0` intentionally causes a CPU exception and is not a normal shell command to run during routine use.

## Project layout

Key files and directories under `w2/` (not an exhaustive inventory):

```text
w2/
├── Makefile
├── include/              # Shared declarations and kernel types
│   ├── types.h
│   ├── kstring.h
│   ├── keyboard.h
│   ├── input.h
│   ├── shell.h
│   └── ...
├── src/
│   ├── loader.asm         # Kernel entry/boot assembly
│   ├── kmain.c            # Kernel initialisation
│   ├── gdt/               # Segmentation setup
│   ├── interrupts/        # IDT, CPU exceptions and assembly stubs
│   ├── keyboard/          # Scancode decoding, IRQ input and event queue
│   ├── input/             # Character input and line editing
│   ├── shell/             # Command dispatch and history work
│   ├── framebuffer/       # VGA text output
│   └── tui/               # Terminal output and cursor operations
└── iso/                   # GRUB/ISO boot files
```

## Building and running

Build from the `w2/` directory on a system with the project's 32-bit compilation and boot dependencies (including **GCC with 32-bit support, NASM, GNU binutils, GRUB/ISO tooling and QEMU**).

```bash
cd w2
make rebuild
make run
```

If VSCode throws an exception about the port selected is being used, try `make run_testmode`, where telnet will be made unavailable, but will let the program run

The Makefile is the source of truth for available targets and QEMU options. If `make run` is not configured for your environment, build the ISO and launch it with your installed QEMU manually; for example:

```bash
qemu-system-i386 -cdrom os.iso -m 32 -boot d
```

For a headless remote Linux host, QEMU can expose a VNC display and be viewed through an SSH tunnel. That is an optional development setup rather than an OS2 requirement.

## Architecture at a glance

```text
GRUB → loader.asm → kmain()
                        ├── GDT / IDT / PIC setup
                        ├── VGA framebuffer + TUI
                        └── Enable keyboard IRQ1

PS/2 keyboard → IRQ1 → scancode decoder → event queue
                                              ↓
                                       getc() / input editor
                                              ↓
                                           shell
                                              ↓
                                      argc/argv dispatcher
                                              ↓
                                       command handler
```

Keyboard events are generated in interrupt context and consumed outside the IRQ handler. This keeps command processing and screen editing out of the interrupt handler.

## Development checklist

**Legend:** `[x]` implemented or reported working in development; `[ ]` incomplete or still requiring verification. Re-test checked items in a clean QEMU build before submission.

### 1. Interrupt infrastructure

- [x] Fixed-width integer aliases in `include/types.h`.
- [x] Flat GDT and GDT load routine.
- [x] PIC remapping and IRQ acknowledgement.
- [x] Packed IDT gates and IDTR; NASM `lidt` loader.
- [x] CPU exception handlers and diagnostic output.
- [x] IRQ1 keyboard stub and PIC unmasking after IDT setup.
- [x] Repeated keyboard input and exception diagnostics tested in QEMU.
- [ ] Add a PIT/IRQ0 handler and system tick counter (optional next hardware milestone).

### 2. Keyboard and buffered input — Worksheet 2 Part 2, Task 2

- [x] PS/2 Set 1 keyboard scancode decoding.
- [x] Shift/Caps Lock, US QWERTY punctuation and symbol mapping.
- [x] IRQ-driven keyboard event queue, with dropped-event diagnostics.
- [x] `getc()` and bounded `readline()` for shell input.
- [x] Enter and Backspace handling; typed characters echoed to the TUI.
- [x] Arrow-key scancodes decoded into special key events.
- [ ] Complete cursor-aware insertion and Backspace within the line.
- [ ] Complete safe editing across line wrapping and screen scrolling.
- [ ] Verify queue concurrency and ensure the legacy polling path is not active alongside IRQ input.

### 3. Interactive terminal — Worksheet 2 Part 2, Task 3

- [x] VGA output, cursor, newline, screen clearing and scrolling.
- [x] Interactive shell prompt and blank-input handling.
- [x] Function-pointer command table with automatic command lookup.
- [x] Basic `argc`/`argv` argument splitting and dispatch.
- [x] Multiple commands: `help`, `clear`, `version`, `echo`, plus debug/test commands.
- [x] Unknown-command message.
- [ ] Finish quoted arguments, escapes and robust option validation.
- [ ] Finish Up/Down history recall and redraw of the current input line.
- [ ] Test long input, editing at buffer capacity, terminal wrapping and scrolling.

### 4. Worksheet 2 Part 1 verification

- [x] Freestanding 32-bit kernel booted through GRUB/QEMU.
- [x] Three-entry GDT.
- [x] VGA text display and hardware cursor.
- [ ] Verify `sum_of_three()` and two additional C functions called from assembly, with evidence.
- [ ] Confirm required framebuffer APIs: 2D `(x, y)` cursor, strings, numbers, colours and clearing.
- [ ] Test cursor bounds, scrolling at the bottom row and modifier-key edge cases.
- [ ] Perform a clean `make rebuild && make run` test.
- [ ] Confirm generated `.o`, `.d`, `.elf` and `.iso` files are excluded from version control where appropriate.

### 5. Documentation and submission

- [x] Update the README with an overview, architecture and current feature status.
- [ ] Add QEMU screenshots of boot, exception handling, keyboard input, `getc()`/`readline()`, and shell commands.
- [ ] Confirm the README's commands, paths and build instructions against the submitted revision.
- [ ] Document any remaining limitations or deliberate worksheet deviations.
- [ ] Verify the final GitHub commit and submission requirements.

## Next steps

1. **Finish `readline_internal()` and `readline_history()`.** Recalling a command with Up/Down should replace the editable line and redraw it without generating a new prompt.
2. **Make line editing consistent.** Maintain separate input length and cursor offset, supporting insertion and Backspace at the cursor.
3. **Test the completed shell.** Check commands with arguments, full buffers, history boundaries, wrapped lines, Shift/Caps Lock and rapid keyboard input.
4. **Capture assessment evidence.** Take QEMU screenshots and verify the specific Worksheet 2 Part 1/2 criteria.
5. **Then add the PIT timer.** Configure IRQ0 and implement `ticks`/`uptime` as the next optional kernel subsystem.

## Current limitations

- Command history has storage and interface work underway but is **not yet confirmed working end to end**.
- The editor's Left/Right movement, mid-line insertion/deletion and screen redraw need further testing and completion.
- The basic argument tokenizer is whitespace-oriented; robust quotation and escaping are future improvements.
- OS2 currently targets the simple 32-bit x86/GRUB/QEMU environment; it is not a general-purpose operating system.

## References

- *The Little Book About OS Development* — especially its treatment of interrupts and input.
- Operating Systems Worksheet 2 Part 1: bootloader, C functions, framebuffer API, documentation and GitHub submission.
- Operating Systems Worksheet 2 Part 2: interrupt-driven keyboard input, `getc()`/`readline()`, a terminal with multiple commands, and documentation.
