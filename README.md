# nothing-os

A tiny educational i386 OS that deliberately does very little — just enough to boot, provide a minimal shell, a tiny filesystem, and a few toy kernel services. It's intended for learning OS internals, experimentation with multiboot/i386 toolchains, and for playing with the DKL module system documented in this repo.

## Quick facts
- Language: C + i386 Assembly
- Target: i686 multiboot-compatible binaries
- Build system: simple Makefile (uses an i686 ELF cross-toolchain)
- License: see LICENSE in the repository

## Features
- Multiboot-compatible boot + stage2 loader (boot/)
- Minimal kernel in C with low-level i386 support (kernel/)
  - IDT/GDT/IRQ/ISR setup
  - Simple kmalloc/safealloc memory subsystem
  - A tiny "nothingfs" filesystem and a small shell
  - Basic keyboard, PIT, PIC, and audio/beeper support
- A small dynamic kernel library (DKL) system described in the included handbook
- Installation/build helper scripts (install/)

## Minimum requirements
- i686-elf cross toolchain (gcc/binutils) or equivalent (the makefile expects i686-elf-gcc, i686-elf-as, i686-elf-ld)
- GNU make
- QEMU (or other x86 emulator) for running the image
- sh/bash for install scripts

## Build (short)
The repository contains a top-level Makefile that builds `myos.bin`.

From the repo root:
```bash
make
qemu-system-i386 -fda myos.bin
```
