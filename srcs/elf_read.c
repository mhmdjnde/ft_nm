/* ************************************************/
/*                                                */
/*                                                */
/*   elf_read.c                                   */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

int	in_file(t_elf *elf, long offset, long size)
{
	if (offset < 0 || size < 0 || offset > elf->size)
		return (0);
	if (size > elf->size - offset)
		return (0);
	return (1);
}

char	*read_string(t_elf *elf, long str_offset, long str_size, long index)
{
	char	*table;
	long	i;

	if (index < 0 || index >= str_size)
		return (NULL);
	if (!in_file(elf, str_offset, str_size))
		return (NULL);
	table = (char *)elf->addr + str_offset;
	i = index;
	while (i < str_size && table[i])
		i++;
	if (i >= str_size)
		return (NULL);
	return (table + index);
}

int	read_section_header(t_elf *elf, int index, t_shdr *sh)
{
	long		offset;
	char		*ptr;
	Elf64_Shdr	sh64;
	Elf32_Shdr	sh32;

	if (index < 0 || index >= elf->section_header_num)
		return (0);
	offset = elf->section_header_offset
		+ (long)index * elf->section_header_size;
	if (!in_file(elf, offset, elf->section_header_size))
		return (0);
	ptr = (char *)elf->addr + offset;
	if (elf->is_64)
	{
		memcpy(&sh64, ptr, sizeof(sh64));
		sh->type = sh64.sh_type;
		sh->flags = (unsigned long)sh64.sh_flags;
		sh->offset = (long)sh64.sh_offset;
		sh->size = (long)sh64.sh_size;
		sh->link = sh64.sh_link;
		sh->entsize = (long)sh64.sh_entsize;
		sh->name = read_string(elf, elf->section_str_offset,
				elf->section_str_size, (long)sh64.sh_name);
	}
	else
	{
		memcpy(&sh32, ptr, sizeof(sh32));
		sh->type = sh32.sh_type;
		sh->flags = (unsigned long)sh32.sh_flags;
		sh->offset = (long)sh32.sh_offset;
		sh->size = (long)sh32.sh_size;
		sh->link = sh32.sh_link;
		sh->entsize = (long)sh32.sh_entsize;
		sh->name = read_string(elf, elf->section_str_offset,
				elf->section_str_size, (long)sh32.sh_name);
	}
	return (1);
}

void	set_section_str_table(t_elf *elf)
{
	t_shdr	sh;

	elf->section_str_offset = -1;
	elf->section_str_size = 0;
	if (!read_section_header(elf, elf->section_str_index, &sh))
		return ;
	if (sh.type != SHT_STRTAB || !in_file(elf, sh.offset, sh.size))
		return ;
	elf->section_str_offset = sh.offset;
	elf->section_str_size = sh.size;
}
