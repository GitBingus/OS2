# w2

# OS2 — TODO

A development checklist for `w2/`, based on Worksheet 2 Parts 1–2 and current project progress.

**Legend:** `[x]` implemented or reported working; `[ ]` still to do or verify. Check off items only after testing them in QEMU.

## Priority 1 — Complete interrupt infrastructure

- [x] Define fixed-width integer aliases in `include/types.h`.
- [x] Define the PIC command/data ports and vector offsets in `include/pic.h`.
- [x] Implement `pic_remap()` (ICW1–ICW4, master `0x20`, slave `0x28`).
- [x] Implement `pic_acknowledge()` for master and slave IRQs.
- [x] Create the packed 8-byte IDT gate descriptor in `include/interrupts.h`.
- [ ] Add a packed **6-byte IDTR operand** (16-bit limit, then 32-bit base).
- [ ] Add compile-time size checks for both IDT entry (8 bytes) and IDTR operand (6 bytes).
- [ ] Implement `idt_set_gate()` to split a handler address into low/high halves, with selector `0x08` and attributes `0x8E`.
- [ ] Allocate and initialise the IDT (typically 256 entries); use `sizeof(idt) - 1` for the IDTR limit.
- [ ] Write an NASM `lidt` wrapper and link its object into the kernel.
- [ ] Install appropriate exception handlers and a keyboard IRQ stub (vector `0x21` / decimal 33).
- [ ] Ensure the IRQ assembly stub saves/restores processor state correctly and returns with `iretd`.
- [ ] Confirm the PIC remains masked and CPU interrupts disabled until valid handlers are installed.
- [ ] Unmask IRQ1 **only when ready**; enable interrupts using `sti` after IDT/PIC/handler setup.
- [ ] Send an End of Interrupt (EOI) after handling IRQ1.
- [ ] Test repeated keyboard input and exception diagnostics in QEMU.

## Priority 2 — Required buffered keyboard input (Worksheet 2 Part 2, Task 2)

- [x] Decode basic keyboard scancodes into key events using `keyboard.c` (polling version).
- [ ] Route keyboard bytes/events from the IRQ handler into a fixed-size buffer rather than drawing from the interrupt context.
- [ ] Implement a circular/ring buffer with explicit empty and full behaviour.
- [ ] Implement `getc()` to remove and return the next buffered character, including an empty-buffer policy.
- [ ] Implement `readline()` using `getc()`, stopping at Enter/newline and respecting caller buffer capacity.
- [ ] Handle Backspace and prevent input-buffer overflow.
- [ ] Coordinate buffer access safely between the IRQ producer and kernel consumer.
- [ ] Retire the old polling path after interrupt input has been tested; avoid two readers of port `0x60`.

## Priority 3 — Required terminal commands (Worksheet 2 Part 2, Task 3)

- [x] Basic VGA TUI functions (text, newline, Backspace, Tab and cursor movement).
- [x] Implement upward screen scrolling; test showed numbered lines after overflow.
- [ ] Display a prompt, e.g. `OS2> `.
- [ ] Collect a complete input line; handle blank input.
- [ ] Parse the command name and any arguments.
- [ ] Implement **at least two** commands from the worksheet (suggestion: `help`, `clear`, `echo`, `version`).
- [ ] Show a useful message for unknown commands.
- [ ] Ensure command strings are null-terminated and array bounds are checked.
- [ ] Test command input/editing over several lines and after scrolling.

## Priority 4 — Part 1 requirements and polish

- [x] Boot a freestanding 32-bit C kernel with GRUB/QEMU.
- [x] Implement and verify a three-entry flat GDT.
- [x] Basic VGA framebuffer output and hardware cursor.
- [ ] Verify Part 1 Task 2 evidence: `sum_of_three()` **and at least two other C functions** called/tested from assembly.
- [ ] Verify the framebuffer exposes a **2D `(x, y)` cursor API**, string output, **number output**, text colour configuration and screen clearing.
- [ ] Add or verify focused tests for cursor bounds, newline-at-bottom, and keyboard modifier keys.
- [ ] Verify the Makefile builds **and links** `pic.c`, `interrupts.c`, and the NASM interrupt/IDT files once created; run `make rebuild && make run`.
- [ ] Keep generated `.o`, `.d`, `.elf` and `.iso` files out of Git via `.gitignore` where appropriate.

## Priority 5 — Documentation and submission

- [ ] Update the repository `README.md` with design explanations and relevant short code snippets.
- [ ] Include QEMU screenshots showing boot, framebuffer API, keyboard input through IRQ1, `getc()`/`readline()`, and terminal commands.
- [ ] Explain how to build, run and test OS2, including the QEMU monitor setup.
- [ ] Document known limitations and any deliberate deviations from worksheet example code.
- [ ] Commit and push working source, README and screenshots to the required GitHub repository; verify submission requirements.

## Optional extensions (after assessed work)

- [ ] Terminal command history using Up/Down.
- [ ] Tab completion.
- [ ] Full UK keyboard symbol layout and key-release edge cases.
- [ ] Scrollback buffer (distinct from ordinary upward scrolling).
- [ ] Additional shell commands and a simple `ls` when a filesystem exists.

## Next small task

**Implement the packed `struct idtr_entry` in `include/interrupts.h`, then write `idt_set_gate()`**. Do not enable interrupts until you have valid IDT entries, safe handlers and the PIC correctly configured.

### Source requirements

- **Worksheet 2 Part 1:** bootloader, C functions, framebuffer API, README and GitHub submission.
- **Worksheet 2 Part 2:** interrupt-driven keyboard display (Task 1), input-buffer API with `getc()` and `readline()` (Task 2), terminal with at least two commands (Task 3), README (20%).
- **Reference:** *The Little Book About OS Development*, especially Chapter 6 for interrupts and input.
