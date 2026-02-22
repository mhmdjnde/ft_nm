Sure, here’s a **10-step roadmap** from zero to finished `ft_nm` (no code, just what to do).

---

### 1. Project setup

* Create the repo, `ft_nm.c` (or several `.c` files), and a proper **Makefile** that builds `ft_nm` (and `libft.a` if you use it).
* Make sure `make`, `make clean`, `make fclean`, `make re` all work.

---

### 2. Handle command-line arguments & files

* Implement the logic to **parse `argv`**: if no file is given, behave like `nm` (usually use `a.out`); otherwise handle **one or multiple filenames**.
* For each filename: try to **open** it, and if it fails, print an error message similar to real `nm` and continue to the next file.

---

### 3. Map the file into memory safely

* For each valid file descriptor, use `fstat` to get the size, then `mmap` to map it in memory (read-only).
* If `mmap` fails or the file is too small to contain a valid ELF header, print an error and skip it.

---

### 4. Validate that the file is ELF (magic + class)

* Check the **ELF magic bytes** at the start of the mapped file; if they don’t match, print the same kind of “file format not recognized” error as `nm`.
* Read `EI_CLASS` to know if it’s **ELF32 or ELF64**, and reject anything else.

---

### 5. Build an abstraction for ELF32 vs ELF64

* Design an internal structure (or at least a clear logic) that can represent “this file’s ELF header, section headers, and section count” **without duplicating all the code** for 32 and 64 bits.
* Decide how you’ll access: base pointer, section header offset, number of sections, section header size, etc., for both classes.

---

### 6. Locate the symbol tables and string tables

* Iterate over the **section headers** to find the symbol table(s): `SHT_SYMTAB` and/or `SHT_DYNSYM`.
* For each symbol table you want to use, find its associated **string table** (via `sh_link`) and store pointers to both (symtab + strtab).

---

### 7. Extract and filter symbols

* Loop over the entries in the symbol table(s): for each symbol, get its **name index**, **value (address)**, **section index**, **binding**, and **type**.
* Skip symbols that shouldn’t be printed (e.g. empty name, depending on how closely you copy GNU `nm`’s behavior).

---

### 8. Compute the symbol type character

* For each symbol, determine the **letter** to display (`T`, `t`, `B`, `b`, `U`, `D`, `R`, etc.) based on:

  * The section it belongs to (text, data, bss, rodata, etc.),
  * Its binding (local vs global),
  * Whether it’s undefined (U), absolute, etc.
* Try to match the real `nm` output as closely as possible for common binaries.

---

### 9. Sort and print like `nm`

* Decide on the **sorting order** (normally alphabetical by symbol name, case-sensitive) to mimic `nm`’s default.
* For multiple input files, print a **file header** before the symbol list when needed, and format:

  * The **address** (hex, padded) or spaces for undefined,
  * Then the symbol type letter,
  * Then the symbol name.

---

### 10. Robustness, edge cases, and (maybe) bonus options

* Add **bounds checks everywhere**: any offset or pointer derived from the file must stay inside `[file_start, file_start + file_size)`. Never trust the file.
* Compare your output with `/usr/bin/nm` on many binaries (executables, `.o`, `.so`, corrupted files).
* Only when this is solid, consider implementing bonus options (`-a`, `-g`, `-u`, `-r`, `-p`) by filtering and changing the sorting/printing logic.


//bonus

### Bonus roadmap — 4 steps

1. **Parse options (`-a -g -u -r -p`)**

   * Scan argv from the start.
   * Collect flags into a small struct (booleans).
   * Stop parsing when you hit the first filename.
   * If an unknown flag appears, print an nm-like error and exit.

2. **Apply symbol filters before printing**

   * `-u`: keep only undefined (`section_index == SHN_UNDEF`).
   * `-g`: keep only external/global (and weak, like nm does).
   * `-a`: include symbols you normally skip (match nm: more “special/internal” symbols that you currently ignore).

3. **Implement ordering modes**

   * Default: your current name sort.
   * `-p`: don’t sort (preserve extraction order).
   * `-r`: reverse the final order (works with both sorted and `-p`).

4. **Test vs real `nm` with a small matrix**

   * Try: single file, multiple files, missing files, stripped binaries, `.o`, `.so`.
   * Try flag combos: `-g`, `-u`, `-p`, `-r`, `-gu`, `-pr`, `-gpr`, etc.
   * Diff outputs and adjust edge-cases (especially what `-a` includes, and `-g` bind rules).

