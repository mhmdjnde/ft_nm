/* ************************************************/
/*                                                */
/*                                                */
/*   extract_symbols.c                            */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

char	*resolve_symbol_name(t_elf *elf, t_symbol *sym,
		unsigned int name_index)
{
	t_shdr	sh;

	if (sym->type == STT_SECTION && name_index == 0)
	{
		if (!read_section_header(elf, (int)sym->section_index, &sh))
			return (NULL);
		return (sh.name);
	}
	return (read_string(elf, elf->symtab_str_offset, elf->symtab_str_size,
			(long)name_index));
}

int	read_one_symbol(t_elf *elf, int index, t_symbol *sym)
{
	long			offset;
	char			*entry;
	unsigned int	name_index;
	Elf64_Sym		sym64;
	Elf32_Sym		sym32;

	if (index == 0)
		return (0);
	offset = elf->symtab_offset + (long)index * elf->symtab_entsize;
	if (!in_file(elf, offset, elf->symtab_entsize))
		return (0);
	entry = (char *)elf->addr + offset;
	if (elf->is_64)
	{
		memcpy(&sym64, entry, sizeof(sym64));
		name_index = sym64.st_name;
		sym->value = (unsigned long)sym64.st_value;
		sym->section_index = sym64.st_shndx;
		sym->bind = ELF64_ST_BIND(sym64.st_info);
		sym->type = ELF64_ST_TYPE(sym64.st_info);
	}
	else
	{
		memcpy(&sym32, entry, sizeof(sym32));
		name_index = sym32.st_name;
		sym->value = (unsigned long)sym32.st_value;
		sym->section_index = sym32.st_shndx;
		sym->bind = ELF32_ST_BIND(sym32.st_info); //global / local / weak
		sym->type = ELF32_ST_TYPE(sym32.st_info); //function / variable...
	}
	sym->name = resolve_symbol_name(elf, sym, name_index);
	if (!sym->name)
		return (0);
	return (1);
}

int	extract_symbols(t_elf *elf, t_symbol **symbols, int *count)
{
	t_symbol	*arr;
	int			i;
	int			idx;

	*symbols = NULL;
	*count = 0;
	if (elf->symtab_offset == -1 || elf->symtab_num <= 0)
		return (1);
	arr = (t_symbol *)malloc(elf->symtab_num * sizeof(t_symbol));
	if (!arr)
		return (0);
	i = 0;
	idx = 0;
	while (i < elf->symtab_num)
	{
		if (read_one_symbol(elf, i, &arr[idx]))
			idx++;
		i++;
	}
	*symbols = arr;
	*count = idx;
	return (1);
}
