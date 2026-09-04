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

int	validate_elf(void *addr, long size)
{
	unsigned char	*e_ident;

	if (size < EI_NIDENT)
		return (0);
	e_ident = (unsigned char *)addr;
	if (e_ident[EI_MAG0] != ELFMAG0
		|| e_ident[EI_MAG1] != ELFMAG1
		|| e_ident[EI_MAG2] != ELFMAG2
		|| e_ident[EI_MAG3] != ELFMAG3)
		return (0);
	if (e_ident[EI_CLASS] != ELFCLASS32 && e_ident[EI_CLASS] != ELFCLASS64)
		return (0);
	if (e_ident[EI_DATA] != ELFDATA2LSB)
		return (0);
	return (1);
}


int	init_elf(void *addr, long size, t_elf *elf)
{
	unsigned char	*e_ident;
	Elf64_Ehdr		hdr64;
	Elf32_Ehdr		hdr32;

	e_ident = (unsigned char *)addr;
	elf->addr = addr;
	elf->size = size;
	elf->section_str_offset = -1;
	elf->section_str_size = 0;
	if (e_ident[EI_CLASS] == ELFCLASS64)
	{
		if (size < (long)sizeof(Elf64_Ehdr))
			return (0);
		memcpy(&hdr64, addr, sizeof(hdr64));
		elf->is_64 = 1;
		elf->section_header_offset = (long)hdr64.e_shoff;
		elf->section_header_num = hdr64.e_shnum;
		elf->section_header_size = hdr64.e_shentsize;
		elf->section_str_index = hdr64.e_shstrndx;
	}
	else
	{
		if (size < (long)sizeof(Elf32_Ehdr))
			return (0);
		memcpy(&hdr32, addr, sizeof(hdr32));
		elf->is_64 = 0;
		elf->section_header_offset = (long)hdr32.e_shoff;
		elf->section_header_num = hdr32.e_shnum;
		elf->section_header_size = hdr32.e_shentsize;
		elf->section_str_index = hdr32.e_shstrndx;
	}
	if (elf->section_header_offset == 0 || elf->section_header_num == 0)
	{
		elf->section_header_num = 0;
		return (1);
	}
	if (elf->is_64 && elf->section_header_size != (int)sizeof(Elf64_Shdr))
		return (0);
	if (!elf->is_64 && elf->section_header_size != (int)sizeof(Elf32_Shdr))
		return (0);
	if (!in_file(elf, elf->section_header_offset,
			(long)elf->section_header_num * elf->section_header_size))
		return (0);
	set_section_str_table(elf);
	return (1);
}
