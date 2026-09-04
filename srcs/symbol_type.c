/* ************************************************/
/*                                                */
/*                                                */
/*   symbol_type.c                                */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

int	starts_with(char *str, char *prefix)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (prefix[i])
	{
		if (str[i] != prefix[i])
			return (0);
		i++;
	}
	return (1);
}

int	is_debug_section(t_shdr *sh)
{
	return (starts_with(sh->name, ".debug")
		|| starts_with(sh->name, ".zdebug")
		|| starts_with(sh->name, ".stab")
		|| starts_with(sh->name, ".gnu.linkonce.wi."));
}

char	decode_section_type(t_shdr *sh)
{
	int	has_contents;
	int	is_read_only;

	has_contents = (sh->type != SHT_NOBITS);
	is_read_only = ((sh->flags & SHF_WRITE) == 0);
	if (sh->flags & SHF_EXECINSTR)
		return ('t');
	if ((sh->flags & SHF_ALLOC) && has_contents)
	{
		if (is_read_only)
			return ('r');
		return ('d');
	}
	if (!has_contents)
		return ('b');
	if (is_debug_section(sh))
		return ('N');
	if (is_read_only)
		return ('n');
	return ('?');
}

char	get_symbol_letter(t_elf *elf, t_symbol *sym)
{
	t_shdr	sh;
	char	c;

	if (sym->section_index == SHN_COMMON)
		return ('C');
	if (sym->section_index == SHN_UNDEF)
	{
		if (sym->bind == STB_WEAK && sym->type == STT_OBJECT)
			return ('v');
		if (sym->bind == STB_WEAK)
			return ('w');
		return ('U');
	}
	if (sym->type == STT_GNU_IFUNC)
		return ('i');
	if (sym->bind == STB_WEAK && sym->type == STT_OBJECT)
		return ('V');
	if (sym->bind == STB_WEAK)
		return ('W');
	if (sym->bind == STB_GNU_UNIQUE)
		return ('u');
	if (sym->section_index == SHN_ABS)
		c = 'a';
	else if (!read_section_header(elf, (int)sym->section_index, &sh))
		return ('?');
	else
		c = decode_section_type(&sh);
	if (sym->bind == STB_GLOBAL && c >= 'a' && c <= 'z')
		c = (char)(c - 'a' + 'A');
	return (c);
}
