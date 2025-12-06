/* ************************************************/
/*                                                */
/*                                                */
/*   elf_validation_and_ini.c                     */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

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
