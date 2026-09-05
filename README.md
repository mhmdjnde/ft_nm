# ft_nm

`ft_nm` is a reimplementation of the Unix `nm` command in C, written without any
external library: the ELF file is mapped into memory and every structure is
decoded by hand from the raw bytes.

It reads 32-bit and 64-bit ELF files (object files, executables, shared
libraries), walks their section header table to find the symbol table, and
prints every symbol the way GNU `nm` does — same columns, same type letters,
same order, same error messages.

```
$ ./ft_nm valid_tests/test1_basic.o
0000000000000000 T add
0000000000000000 D g1
0000000000000004 d s1
```

---

## Build and run

```sh
make
```

Produces `./ft_nm` at the root. `make clean`, `make fclean` and `make re` behave
as usual. The binary and the object files are gitignored and rebuilt on demand.

```sh
./ft_nm [options] [files...]
```

With no file argument, `ft_nm` reads `a.out`, like `nm`.

## Options

The bonus options are implemented, and can be given in any order, grouped
(`-agu`) or separate, before or after the file names. A bare `--` ends the
options, so everything after it is treated as a file name even if it starts
with `-`.

| option | effect |
|---|---|
| `-a` | also print the debugger-only symbols (`STT_FILE` and `STT_SECTION` entries), which are hidden by default |
| `-g` | print only the external symbols (global, weak, unique, undefined and common) |
| `-u` | print only the undefined symbols |
| `-r` | reverse the sort order |
| `-p` | do not sort at all, keep the order of the symbol table |

When several files are given, each listing is preceded by a blank line and a
`file:` header, exactly like `nm`.

## What gets listed

Like `nm` without `-D`, only the **static** symbol table `.symtab` is read. A
stripped binary or a shared library, where only `.dynsym` survives, is therefore
reported as having no symbols:

```
$ ./ft_nm /bin/ls
ft_nm: /bin/ls: no symbols
```

The entry at index 0 of the symbol table is the reserved null entry and is
always skipped.

Symbols are sorted by name with `strcoll`, so the order follows the machine's
`LC_COLLATE` setting — that is why `main` sets `setlocale(LC_COLLATE, "")`
before anything else. Without it the sort would be plain ASCII and would
diverge from `nm` on names containing `_` or digits.

## Output format

Each line is `address`, `type letter`, `name`. The address column is 16 hex
digits for an ELF64 file and 8 for an ELF32 one, and is left blank for symbols
that have no address (`U`, `w`, `v`).

The type letter is computed in [symbol_type.c](srcs/symbol_type.c): special
section indexes and bindings are handled first (common, undefined, ifunc, weak,
unique), otherwise the letter comes from the flags of the section the symbol
lives in — executable → `t`, allocated and writable → `d`, allocated and
read-only → `r`, `SHT_NOBITS` → `b`, a `.debug`/`.stab` section → `N`. The
letter is then upper-cased if the symbol is global, which is where the whole
`A a B b C D d i N n R r T t U V v W w u ?` set comes from.

The full list with a one-line description of each letter is in
[nm_output_types.txt](valid_tests/nm_output_types.txt).

## How it works

[nm_path.svg](nm_path.svg) traces the whole path visually, from `open()` to one
printed line. In text, the pipeline is:

1. **Map the file** — [ft_nm.c](srcs/ft_nm.c) opens it, uses `fstat` to get the
   size (also catching directories and empty files) and `mmap` to map it
   read-only. All the reading afterwards is pointer arithmetic inside that
   mapping.
2. **Validate** — [elf_validation_and_ini.c](srcs/elf_validation_and_ini.c)
   checks the magic bytes, the class (ELF32 or ELF64) and the endianness before
   anything else is interpreted.
3. **Initialise** — the same file copies the ELF header into a common `t_elf`
   structure holding the section header table's offset, count and entry size,
   plus the location of `.shstrtab` (the section-name string table). From this
   point on, nothing else in the program needs to know whether the file is 32-
   or 64-bit.
4. **Read sections** — [elf_read.c](srcs/elf_read.c) provides the three
   primitives everything else is built on: `in_file()` (bounds check),
   `read_string()` (safe string-table lookup) and `read_section_header()`, which
   normalises an `Elf32_Shdr` or an `Elf64_Shdr` into the neutral `t_shdr`.
5. **Find the symbol table** —
   [find_symbol_tables.c](srcs/find_symbol_tables.c) scans the section headers
   for the first usable `SHT_SYMTAB`, checking its entry size and that its
   `sh_link` really points at a string table before keeping it.
6. **Extract the symbols** —
   [extract_symbols.c](srcs/extract_symbols.c) decodes each entry into a
   `t_symbol` (name, value, section index, bind, type). Section symbols with an
   empty name take the name of the section they refer to.
7. **Filter, sort, print** — [options.c](srcs/options.c) applies the option
   flags, then [print_symbols.c](srcs/print_symbols.c) sorts and writes the
   lines with `write` only.

## Robustness

The input is untrusted, so a corrupted or hand-crafted file must never make
`ft_nm` read outside its mapping:

-   Every offset and size taken from the file goes through `in_file()` before
    being used.
-   They are all kept in a signed 64-bit `long`, never an `int`. A 64-bit ELF
    field can hold a huge value, and truncating it to an `int` would silently
    turn it into a small or negative number that would pass the checks.
-   String-table entries are only returned once their terminating `\0` has been
    found *inside* the table, so a name can never run past the end of the
    mapping.
-   Every ELF structure is copied out with `memcpy` instead of being read
    through a cast, so a misaligned offset can never cause an unaligned access.

The `bad_sect_tbl.o` fixture is a good illustration: the section header table
offset is overwritten with `0xFFFFFFFFFFFFFFFF`. GNU `nm` reacts with an
`out of memory allocating ... bytes` message, while `ft_nm` rejects the file
cleanly as an unrecognised format.

## Error handling

Messages and exit codes follow `nm`:

| case | output | exit |
|---|---|---|
| missing file | `ft_nm: 'x': No such file` | 1 |
| directory | `ft_nm: Warning: 'x' is a directory` | 1 |
| unreadable file | `ft_nm: x: Permission denied` | 1 |
| empty file | *(nothing, like `nm`)* | 1 |
| not an ELF file | `ft_nm: x: file format not recognized` | 1 |
| ELF without symbols | `ft_nm: x: no symbols` | 0 |
| unknown option | `ft_nm: invalid option -- 'z'` + usage | 1 |

With several files, one failing file sets the exit status to 1 but the others
are still listed.

## Tests

Two fixture sets, each with its own `Makefile`.

**`valid_tests/`** — ten small C files covering the interesting cases: plain
globals and statics, BSS, rodata, static functions, weak symbols, externs,
multiple translation units, structs, static data and function pointers. Their
`.o` files are committed on purpose, since they are the exact fixtures the
reference outputs were taken from.

```sh
make -C valid_tests
```

Then compare against the real `nm` — this loop should print nothing:

```sh
cd valid_tests && for f in *.o; do for o in "" -a -g -u -r -p -agr; do diff <(nm $o $f) <(../ft_nm $o $f) > /dev/null || echo "DIFF: $o $f"; done; done
```

[ForLazyEvaluators.txt](valid_tests/ForLazyEvaluators.txt) holds a saved
`nm` vs `ft_nm` transcript for these files, for a quick read-only check.

**`invalid_tests/`** — malformed inputs generated from scratch by the Makefile
(a 2-byte file, a text file, broken magic, an invalid class byte, a truncated
header, a corrupted section header table offset). They are gitignored and built
on demand:

```sh
make -C invalid_tests
for f in invalid_tests/*.o; do ./ft_nm $f; done
```

## Layout

```
Makefile              builds ./ft_nm
srcs/ft_nm.h          the t_elf, t_shdr, t_symbol and t_opts structures
srcs/ft_nm.c          main, argument loop, open/fstat/mmap, per-file driver
srcs/options.c        option parsing, usage, symbol filtering
srcs/elf_validation_and_ini.c   magic/class/endianness checks, ELF header
srcs/elf_read.c       bounds checks, string tables, section headers
srcs/find_symbol_tables.c       locates and validates .symtab and its .strtab
srcs/extract_symbols.c          decodes symbol entries and resolves names
srcs/symbol_type.c    the nm type letter
srcs/print_symbols.c  sorting and output
nm_path.svg           diagram of the full path, from open() to one line
valid_tests/          well-formed fixtures + reference outputs
invalid_tests/        malformed fixtures, generated by its Makefile
```
