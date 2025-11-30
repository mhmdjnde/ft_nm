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

/*
** init_elf:
**   - Interprets the mapped file as either an ELF64 or ELF32 header,
**     based on EI_CLASS (already checked in validate_elf).
**   - Checks that the full ELF header fits in the file.
**   - Checks that the section header table (e_shoff, e_shnum, e_shentsize)
**     is fully contained inside the file.
**   - Checks that the section header entry size matches the expected
**     sizeof(Elf64_Shdr) or sizeof(Elf32_Shdr).
**   - Fills the t_elf structure with:
**       * addr, size        : base pointer and file size
**       * is_64             : 1 for ELF64, 0 for ELF32
**       * section_header_*  : offset, number, entry size
**       * section_str_index : index of the section name string table
**   - Returns 1 if everything is OK, 0 on any error (and prints an error).
*/
//added by jnde
//the e_shoff is where the header table is started (offset, usually 0x400)
//the e_shnum is the number of section entries, how many entries ther eis
//the e_shentsize is the entry size
int	init_elf(void *addr, int size, t_elf *elf)
{
	char		*e_ident;
	Elf64_Ehdr	*hdr64;
	Elf32_Ehdr	*hdr32;

	e_ident = (char *)addr;
	elf->addr = addr;
	elf->size = size;
	if (e_ident[EI_CLASS] == ELFCLASS64)
	{
		if (size < (int)sizeof(Elf64_Ehdr))
		{
			write(2, "nm: corrupted ELF header\n", 25);
			return (0);
		}
		hdr64 = (Elf64_Ehdr *)addr;
		if ((int)hdr64->e_shoff < 0
			|| (int)hdr64->e_shoff >= size
			|| (int)hdr64->e_shoff + hdr64->e_shnum * hdr64->e_shentsize > size)
		{
			write(2, "nm: invalid section header table\n", 33);
			return (0);
		}
		if (hdr64->e_shentsize != sizeof(Elf64_Shdr))
		{
			write(2, "nm: unsupported section header size\n", 36);
			return (0);
		}
		elf->is_64 = 1;
		elf->section_header_offset = (int)hdr64->e_shoff;
		elf->section_header_num = hdr64->e_shnum;
		elf->section_header_size = hdr64->e_shentsize;
		elf->section_str_index = hdr64->e_shstrndx;
	}
	else
	{
		if (size < (int)sizeof(Elf32_Ehdr))
		{
			write(2, "nm: corrupted ELF header\n", 25);
			return (0);
		}
		hdr32 = (Elf32_Ehdr *)addr;
		if ((int)hdr32->e_shoff < 0
			|| (int)hdr32->e_shoff >= size
			|| (int)hdr32->e_shoff + hdr32->e_shnum * hdr32->e_shentsize > size)
		{
			write(2, "nm: invalid section header table\n", 33);
			return (0);
		}
		if (hdr32->e_shentsize != sizeof(Elf32_Shdr))
		{
			write(2, "nm: unsupported section header size\n", 36);
			return (0);
		}
		elf->is_64 = 0;
		elf->section_header_offset = (int)hdr32->e_shoff;
		elf->section_header_num = hdr32->e_shnum;
		elf->section_header_size = hdr32->e_shentsize;
		elf->section_str_index = hdr32->e_shstrndx;
	}
	return (1);
}

/*
** validate_elf:
**   - Works on the beginning of the mapped file (addr).
**   - First checks that the file is at least EI_NIDENT (which is 16) bytes long,
**     so it can contain the ELF identification array e_ident[].
**   - Then checks the 4 magic bytes (0x7F, 'E', 'L', 'F').
**   - Then checks that EI_CLASS is either ELFCLASS32 or ELFCLASS64.
**   - Returns 1 if the file looks like a supported ELF, 0 otherwise
**     (and prints an appropriate error message).
*/
int	validate_elf(void *addr, size_t size)
{
	char	*e_ident;

	if (size < EI_NIDENT)
	{
		write(2, "nm: file too small to be an ELF file\n", 37);
		return (0);
	}
	e_ident = (char *)addr;
	if (e_ident[EI_MAG0] != ELFMAG0
		|| e_ident[EI_MAG1] != ELFMAG1
		|| e_ident[EI_MAG2] != ELFMAG2
		|| e_ident[EI_MAG3] != ELFMAG3)
	{
		write(2, "nm: file format not recognized\n", 31);
		return (0);
	}
	if (e_ident[EI_CLASS] != ELFCLASS32 && e_ident[EI_CLASS] != ELFCLASS64)
	{
		write(2, "nm: unsupported ELF class\n", 26);
		return (0);
	}
	return (1);
}
