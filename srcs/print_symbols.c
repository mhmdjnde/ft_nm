/* ************************************************/
/*                                                */
/*                                                */
/*   print_symbols.c                              */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

void	put_hex_padded(unsigned long value, int width)
{
	char	buf[16];
	int		i;

	if (width > 16)
		width = 16;
	i = width - 1;
	while (i >= 0)
	{
		buf[i] = "0123456789abcdef"[value % 16];
		value = value / 16;
		i--;
	}
	write(1, buf, width);
}

char	get_symbol_letter(t_elf *elf, t_symbol *sym)
{
	unsigned char	bind;
	unsigned char	type;
	unsigned short	shndx;
	unsigned long	sh_type;
	unsigned long	sh_flags;
	char			c;
	char			*base;

	bind = sym->bind;
	type = sym->type;
	shndx = sym->section_index;
	if (shndx == SHN_UNDEF)
	{
		if (bind == STB_WEAK)
		{
			if (type == STT_OBJECT)
				c = 'v';
			else
				c = 'w';
		}
		else
			c = 'u';
	}
	else if (shndx == SHN_ABS)
		c = 'a';
	else if (shndx == SHN_COMMON)
		c = 'c';
	else
	{
		if (shndx >= elf->section_header_num)
			return ('?');
		base = (char *)elf->addr;
		if (elf->is_64)
		{
			Elf64_Shdr	*sh;

			sh = (Elf64_Shdr *)(base + elf->section_header_offset
					+ shndx * elf->section_header_size);
			sh_type = sh->sh_type;
			sh_flags = sh->sh_flags;
		}
		else
		{
			Elf32_Shdr	*sh;

			sh = (Elf32_Shdr *)(base + elf->section_header_offset
					+ shndx * elf->section_header_size);
			sh_type = sh->sh_type;
			sh_flags = sh->sh_flags;
		}
		if (sh_type == SHT_NOBITS
			&& (sh_flags & SHF_ALLOC) && (sh_flags & SHF_WRITE))
			c = 'b';
		else if (sh_type == SHT_PROGBITS
			&& (sh_flags & SHF_ALLOC) && (sh_flags & SHF_EXECINSTR))
			c = 't';
		else if (sh_type == SHT_PROGBITS
			&& (sh_flags & SHF_ALLOC) && (sh_flags & SHF_WRITE))
			c = 'd';
		else if (sh_type == SHT_PROGBITS
			&& (sh_flags & SHF_ALLOC) && !(sh_flags & SHF_WRITE))
			c = 'r';
		else if (sh_type == SHT_DYNAMIC)
			c = 'd';
		else
			c = 't';
		if (bind == STB_WEAK)
		{
			if (type == STT_OBJECT)
				c = 'v';
			else
				c = 'w';
		}
	}
	if (bind == STB_GLOBAL || bind == STB_WEAK)
	{
		if (c >= 'a' && c <= 'z')
			c = c - 'a' + 'A';
	}
	return (c);
}

void	sort_and_print_symbols(t_elf *elf, t_symbol *symbols, int count)
{
	int			i;
	int			j;
	int			min;
	t_symbol	tmp;
	char		c;
	int			width;

	i = 0;
	while (i < count - 1)
	{
		min = i;
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(symbols[j].name, symbols[min].name) < 0)
				min = j;
			j++;
		}
		if (min != i)
		{
			tmp = symbols[i];
			symbols[i] = symbols[min];
			symbols[min] = tmp;
		}
		i++;
	}
	width = 8;
	if (elf->is_64)
		width = 16;
	i = 0;
	while (i < count)
	{
		c = get_symbol_letter(elf, &symbols[i]);
		if (symbols[i].section_index == SHN_UNDEF)
		{
			j = 0;
			while (j < width)
			{
				write(1, " ", 1);
				j++;
			}
		}
		else
			put_hex_padded(symbols[i].value, width);
		write(1, " ", 1);
		write(1, &c, 1);
		write(1, " ", 1);
		write(1, symbols[i].name, ft_strlen(symbols[i].name));
		write(1, "\n", 1);
		i++;
	}
}
