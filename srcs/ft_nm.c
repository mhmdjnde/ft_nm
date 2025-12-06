/* ************************************************/
/*                                                */
/*                                                */
/*   ft_nm.c                                      */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

// road map
// File names → file descriptors (`open` in `check_files`)
// File descriptors → bytes in memory (`fstat` + `mmap` in `map_file`)
// Bytes in memory → ELF check (magic + class in `validate_elf`)
// ELF header → section header table info (`init_elf`)
// Section headers → symbol tables + string tables (`find_symbol_tables`)
// Symbol tables + string tables → flat list of symbols (`extract_symbols`)
// Symbol list → type letter + sorted nm-style output (`sort_and_print_symbols`)

// We take the file path, open it with open and get its file descriptor.
// After checking that the file exists, we use fstat to get the file size,
// then mmap to load the file bytes into memory as a contiguous block we can read.
// mmap takes the mapping mode (here: read-only, PROT_READ) and returns an address
// as a void * that we then treat as a char * to read the raw bytes.

// After a successful mmap, we validate that the file is an ELF file (ELF32 or ELF64)
// using that address. To do that, we first make sure the file is at least 16 bytes
// (so it can contain the e_ident array) and that the first 4 bytes match the ELF
// magic (0x7F, 'E', 'L', 'F').

// Once we know it is a valid ELF file, we initialize the information we need about it:
// the class (32-bit or 64-bit), the section header offset (e_shoff), the number of
// section headers (e_shnum), the size of each section header (e_shentsize), and the
// index of the section-name string table (e_shstrndx).

// After this initialization, we iterate over each section header. We look for section
// headers whose type is a symbol table (SHT_SYMTAB or SHT_DYNSYM). For each
// symbol-table section, we read its symbol offset, size, entry size, and the sh_link
// field, which points to its associated string-table section. Then, using that link,
// we locate the string-table section header, get its offset and size, and check that
// all offsets and sizes stay inside the file and that its type is SHT_STRTAB.

// If everything is valid, and the section type is SHT_SYMTAB or SHT_DYNSYM, we store
// the symbol-table offsets/sizes and the corresponding string-table offsets/sizes
// into the main t_elf struct, so we can later read the symbols and their names.

// Next, in extract_symbols, we walk through each entry in the symbol tables we found,
// use the string-table offsets to resolve the symbol names, and fill a flat array of
// t_symbol with: name, value (address), section index, binding, type, and whether it
// comes from .symtab or .dynsym. We can skip unwanted symbols here (e.g. empty names).

// Finally, in sort_and_print_symbols, we compute the nm type letter for each symbol
// by combining its binding, type, and the flags/type of its section header
// (text/data/bss/rodata, etc.), then sort all symbols by name and print them in
// the same format as nm: [address or spaces] [type letter] [symbol name].


int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	*map_file(int fd, size_t *size)
{
	struct stat	st;
	void		*addr;

	if (fstat(fd, &st) < 0)
	{
		write(2, "nm: fstat failed\n", 17);
		return (NULL);
	}
	if (st.st_size < 4)
	{
		write(2, "nm: file too small to be an ELF file\n", 37);
		return (NULL);
	}
	addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (addr == MAP_FAILED)
	{
		write(2, "nm: mmap failed\n", 16);
		return (NULL);
	}
	*size = (size_t)st.st_size;
	return (addr);
}

int	*check_files(char **files)
{
	int	files_i;
	int	fds_i;
	int	len;
	int	fd;
	int	*fds;

	files_i = 0;
	fds_i = 0;
	len = 0;
	while (files[len] != NULL)
		len++;
	fds = malloc((len + 1) * sizeof(int));
	if (!fds)
		return (NULL);
	while (files[files_i] != NULL)
	{
		fd = open(files[files_i], O_RDONLY);
		if (fd < 0)
		{
			write(2, "nm: '", 5);
			write(2, files[files_i], ft_strlen(files[files_i]));
			write(2, "': No such file\n", 16);
		}
		else
		{
			fds[fds_i] = fd;
			fds_i++;
		}
		files_i++;
	}
	fds[fds_i] = -1;
	return (fds);
}

int	main(int argc, char **argv)
{
	int			*fds;
	int			i;
	void		*addr;
	size_t		size;
	t_elf		elf;
	t_symbol	*symbols;
	int			symbol_count;
	char		*types;

	if (argc < 2)
	{
		write(2, "Usage: ./ft_nm <file> [file...]\n", 32);
		return (1);
	}
	fds = check_files(&argv[1]);
	if (!fds)
		return (1);
	i = 0;
	while (fds[i] != -1)
	{
		addr = map_file(fds[i], &size);
		if (addr != NULL)
		{
			if (validate_elf(addr, size) && init_elf(addr, (int)size, &elf))
			{
				if (find_symbol_tables(&elf)
					&& extract_symbols(&elf, &symbols, &symbol_count))
				{
					types = (char *)malloc(symbol_count * sizeof(char));
					if (!types)
					{
						free(symbols);
					}
					else
					{
						compute_symbol_types(&elf, symbols, symbol_count, types);
						free(types);
						free(symbols);
					}
				}
			}
			munmap(addr, size);
		}
		close(fds[i]);
		i++;
	}
	free(fds);
	return (0);
}
