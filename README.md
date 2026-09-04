# ft_nm

`ft_nm` is a reimplementation of the Unix `nm` command in C.  
It parses 32-bit and 64-bit ELF files (executables, object files and shared libraries) and displays their symbols in a way similar to GNU `nm`.

---

## Features

-   Handles **ELF 32-bit (x86)** and **ELF 64-bit (x86-64)** binaries.
-   Uses `fstat` + `mmap` to safely map files into memory.
-   Validates ELF magic, class and endianness before reading anything.
-   Abstracts ELF32 / ELF64 via a common `t_elf` structure, plus a neutral
    `t_shdr` so no other file has to know which class the file is.
-   Locates **symbol tables** (`.symtab`, `.dynsym`) and their **string tables**.
-   Prints, for every symbol:
    -   the address (`16` hex digits for ELF64, `8` for ELF32, blanks when undefined)
    -   the type letter (`A a B b C D d i N n R r T t U V v W w u ?`)
    -   the symbol name
-   Sorts like `nm` (with `strcoll`, so the order follows `LC_COLLATE`) and
    handles multiple files, printing a `file:` header before each one.
-   Bonus options: `-a`, `-g`, `-u`, `-r`, `-p`, in any order, grouped
    (`-agu`) or not, before or after the file names, with `--` ending them.

Like `nm` without `-D`, only the static symbol table `.symtab` is listed: a
stripped binary or shared library, where only `.dynsym` is left, is reported as
having no symbols.

## Error handling

Every offset and size read from the file is checked against the mapped size
before it is used, string table entries are only accepted once their terminating
`\0` has been seen inside the table, and every ELF structure is copied out with
`memcpy` rather than read through a cast, so a corrupted offset can never cause
an unaligned access. Errors follow `nm`:

| case | output | exit |
|---|---|---|
| missing file | `ft_nm: 'x': No such file` | 1 |
| directory | `ft_nm: Warning: 'x' is a directory` | 1 |
| unreadable file | `ft_nm: x: Permission denied` | 1 |
| empty file | *(nothing, like `nm`)* | 1 |
| not an ELF file | `ft_nm: x: file format not recognized` | 1 |
| ELF without symbols | `ft_nm: x: no symbols` | 0 |
| unknown option | `ft_nm: invalid option -- 'z'` + usage | 1 |

---

## Build

```sh
make
```

## Compare with the real `nm`

```sh
diff <(nm -a some_file.o) <(./ft_nm -a some_file.o)
```
