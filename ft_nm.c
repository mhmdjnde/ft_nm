/* ************************************************/
/*                                                */
/*                                                */
/*   ft_nm.c                                      */
/*                                                */
/*   By: JndeFromHome <jnde@student.jnde.fr>      */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

//road map
// File names → file descriptors (`open` in `check_files`)
// File descriptors → bytes in memory (`fstat` + `mmap` in `map_file`)
// Bytes in memory → ELF check (magic + class in `validate_elf`)
// ELF header → section header table info (`init_elf`)
// Section headers → symbol tables + string tables (`find_symbol_tables`)

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

/*
** map_file:
**   - Uses fstat(fd, &st) to get the file size in st.st_size.
**   - Rejects files smaller than 4 bytes (too small for an ELF header).
**   - Uses mmap to map the whole file into memory:
**       addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
**     * mmap creates a memory mapping of the file contents.
**     * mmap returns a pointer (addr) that can be treated as a char*
**       to read the file bytes directly from memory.
**   - On success:
**       * stores the file size in *size
**       * returns addr
**   - On error (fstat or mmap failure):
**       * prints an error and returns NULL.
*/
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

/*
** check_files:
**   - Takes a NULL-terminated array of filenames (files).
**   - Counts how many filenames there are.
**   - Allocates an int array of size (len + 1) to store file descriptors.
**   - For each filename:
**       * tries to open it read-only with open(name, O_RDONLY)
**       * on failure: prints "nm: '<name>': No such file"
**       * on success: stores the file descriptor in the fds array
**   - Terminates the array with -1 as a sentinel.
**   - Returns the allocated fds array (or NULL if malloc fails).
*/
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

/*
** main:
**   - Checks that at least one filename is provided on the command line.
**   - Uses check_files(&argv[1]) to open all requested files and collect
**     valid file descriptors in a NULL-terminated int array (ending with -1).
**   - For each valid file descriptor:
**       1) map_file(fd, &size):
**          * fstat + mmap the whole file in read-only memory
**          * returns addr (base pointer) and size (file size in bytes)
**       2) validate_elf(addr, size):
**          * check ELF magic and class (ELF32 / ELF64)
**       3) init_elf(addr, (int)size, &elf):
**          * interpret ELF header as 32 or 64-bit
**          * validate section header table
**          * store key info in the t_elf struct
**       4) later, you will use the t_elf information to locate sections,
**          symbol tables, string tables, and finally print symbols.
**       5) munmap(addr, size) releases the mapping created by mmap.
**          After munmap, addr must not be used anymore.
**       6) close(fd) closes the file descriptor.
**   - Finally frees the fds array and returns 0.
*/
int	main(int argc, char **argv)
{
	int		*fds;
	int		i;
	void	*addr;
	size_t	size;
	t_elf	elf;

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
			if (validate_elf(addr, size))
			{
				if (init_elf(addr, (int)size, &elf))
				{
					if (find_symbol_tables(&elf))
					{
						/* later:
						**   use elf.symtab_* and elf.dynsym_*
						**   to loop over symbols and print them like nm
						*/
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

