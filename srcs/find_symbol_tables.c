/* ************************************************/
/*                                                */
/*                                                */
/*   find_symbol_tables.c                         */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

int	is_valid_symbol_table(t_elf *elf, t_shdr *sh, t_shdr *str_sh)
{
	long	entsize;

	entsize = (long)sizeof(Elf32_Sym);
	if (elf->is_64)
		entsize = (long)sizeof(Elf64_Sym);
	if (sh->entsize != entsize || sh->size < entsize)
		return (0);
	if (!in_file(elf, sh->offset, sh->size))
		return (0);
	if (sh->size / entsize > 2147483000)
		return (0);
	if (!read_section_header(elf, (int)sh->link, str_sh))
		return (0);
	if (str_sh->type != SHT_STRTAB
		|| !in_file(elf, str_sh->offset, str_sh->size))
		return (0);
	return (1);
}

void	keep_symbol_table(t_elf *elf, t_shdr *sh, t_shdr *str_sh)
{
	elf->symtab_offset = sh->offset;
	elf->symtab_entsize = (int)sh->entsize;
	elf->symtab_num = (int)(sh->size / sh->entsize);
	elf->symtab_str_offset = str_sh->offset;
	elf->symtab_str_size = str_sh->size;
}

int	find_symbol_tables(t_elf *elf)
{
	t_shdr	sh;
	t_shdr	str_sh;
	int		i;

	elf->symtab_offset = -1;
	elf->symtab_num = 0;
	elf->symtab_entsize = 0;
	elf->symtab_str_offset = -1;
	elf->symtab_str_size = 0;
	i = 0;
	while (i < elf->section_header_num)
	{
		if (!read_section_header(elf, i, &sh))
			return (0);
		if (sh.type == SHT_SYMTAB && elf->symtab_offset == -1
			&& is_valid_symbol_table(elf, &sh, &str_sh))
			keep_symbol_table(elf, &sh, &str_sh);
		i++;
	}
	return (1);
}
