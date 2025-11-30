# ft_nm

`ft_nm` is a reimplementation of the Unix `nm` command in C.  
It parses 32-bit and 64-bit ELF files (executables, object files and shared libraries) and displays their symbols in a way similar to GNU `nm`.

---

## Features

-   Handles **ELF 32-bit (x86)** and **ELF 64-bit (x86-64)** binaries.
-   Uses `fstat` + `mmap` to safely map files into memory.
-   Validates ELF magic and class before reading anything.
-   Abstracts ELF32 / ELF64 via a common `t_elf` structure.
-   Locates **symbol tables** (`.symtab`, `.dynsym`) and their **string tables**.

Planned:

-   Extract symbols and print:
    -   address
    -   type letter (`T`, `t`, `U`, `B`, `D`, `R`, …)
    -   symbol name
-   Sort symbols like `nm` and handle multiple files.
-   Optional bonus flags (`-a`, `-g`, `-u`, `-r`, `-p`).

---

## Build

```sh
make
```
