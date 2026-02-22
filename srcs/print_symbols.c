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

int	extract_symbols(t_elf *elf, t_symbol **symbols, int *count)
{
	char		*base;
	int			max_syms;
	t_symbol	*arr;
	int			i;
	int			idx;

	base = (char *)elf->addr;
	*symbols = NULL;
	*count = 0;
	max_syms = 0;
	if (elf->symtab_offset != -1 && elf->symtab_num > 0)
		max_syms += elf->symtab_num;
	if (elf->dynsym_offset != -1 && elf->dynsym_num > 0)
		max_syms += elf->dynsym_num;
	if (max_syms <= 0)
	{
		write(2, "nm: no symbol table to read\n", 28);
		return (0);
	}
	arr = (t_symbol *)malloc(max_syms * sizeof(t_symbol));
	if (!arr)
		return (0);
	idx = 0;
	if (elf->symtab_offset != -1 && elf->symtab_num > 0)
	{
		int	sym_off = elf->symtab_offset;
		int	sym_entsize = elf->symtab_entsize;
		int	str_off = elf->symtab_str_offset;
		int	str_size = elf->symtab_str_size;

		i = 0;
		while (i < elf->symtab_num)
		{
			int	entry_off = sym_off + i * sym_entsize;

			if (entry_off < 0 || entry_off + sym_entsize > elf->size)
			{
				write(2, "nm: symbol entry out of file\n", 29);
				break ;
			}
			if (elf->is_64)
			{
				Elf64_Sym	*sym64;
				int			name_index;

				sym64 = (Elf64_Sym *)(base + entry_off);
				name_index = (int)sym64->st_name;
				if (name_index > 0 && name_index < str_size)
				{
					char	*name;

					name = base + str_off + name_index;
					if (*name)
					{
						arr[idx].name = name;
						arr[idx].value = (unsigned long)sym64->st_value;
						arr[idx].section_index = sym64->st_shndx;
						arr[idx].bind = ELF64_ST_BIND(sym64->st_info);
						arr[idx].type = ELF64_ST_TYPE(sym64->st_info);
						arr[idx].is_dyn = 0;
						idx++;
					}
				}
			}
			else
			{
				Elf32_Sym	*sym32;
				int			name_index;

				sym32 = (Elf32_Sym *)(base + entry_off);
				name_index = (int)sym32->st_name;
				if (name_index > 0 && name_index < str_size)
				{
					char	*name;

					name = base + str_off + name_index;
					if (*name)
					{
						arr[idx].name = name;
						arr[idx].value = (unsigned long)sym32->st_value;
						arr[idx].section_index = sym32->st_shndx;
						arr[idx].bind = ELF32_ST_BIND(sym32->st_info);
						arr[idx].type = ELF32_ST_TYPE(sym32->st_info);
						arr[idx].is_dyn = 0;
						idx++;
					}
				}
			}
			i++;
		}
	}
	if (elf->dynsym_offset != -1 && elf->dynsym_num > 0)
	{
		int	sym_off = elf->dynsym_offset;
		int	sym_entsize = elf->dynsym_entsize;
		int	str_off = elf->dynsym_str_offset;
		int	str_size = elf->dynsym_str_size;

		i = 0;
		while (i < elf->dynsym_num)
		{
			int	entry_off = sym_off + i * sym_entsize;

			if (entry_off < 0 || entry_off + sym_entsize > elf->size)
			{
				write(2, "nm: dynamic symbol entry out of file\n", 37);
				break ;
			}
			if (elf->is_64)
			{
				Elf64_Sym	*sym64;
				int			name_index;

				sym64 = (Elf64_Sym *)(base + entry_off);
				name_index = (int)sym64->st_name;
				if (name_index > 0 && name_index < str_size)
				{
					char	*name;

					name = base + str_off + name_index;
					if (*name)
					{
						arr[idx].name = name;
						arr[idx].value = (unsigned long)sym64->st_value;
						arr[idx].section_index = sym64->st_shndx;
						arr[idx].bind = ELF64_ST_BIND(sym64->st_info);
						arr[idx].type = ELF64_ST_TYPE(sym64->st_info);
						arr[idx].is_dyn = 1;
						idx++;
					}
				}
			}
			else
			{
				Elf32_Sym	*sym32;
				int			name_index;

				sym32 = (Elf32_Sym *)(base + entry_off);
				name_index = (int)sym32->st_name;
				if (name_index > 0 && name_index < str_size)
				{
					char	*name;

					name = base + str_off + name_index;
					if (*name)
					{
						arr[idx].name = name;
						arr[idx].value = (unsigned long)sym32->st_value;
						arr[idx].section_index = sym32->st_shndx;
						arr[idx].bind = ELF32_ST_BIND(sym32->st_info);
						arr[idx].type = ELF32_ST_TYPE(sym32->st_info);
						arr[idx].is_dyn = 1;
						idx++;
					}
				}
			}
			i++;
		}
	}
	if (idx == 0)
	{
		free(arr);
		write(2, "nm: no valid symbols found\n", 27);
		return (0);
	}
	*symbols = arr;
	*count = idx;
	return (1);
}

void	sort_symbols(t_symbol *symbols, int count)
{
	int			i;
	int			j;
	int			min;
	t_symbol	tmp;

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
}

int	bounded_strlen(char *s, char *end)
{
	char	*p;

	if (!s || s >= end)
		return (0);
	p = s;
	while (p < end && *p)
		p++;
	return ((int)(p - s));
}

void	print_symbols(t_elf *elf, t_symbol *symbols, int count)
{
	int			i;
	int			j;
	char		c;
	int			width;
	char		*end;
	int		len;

	width = 8;
	if (elf->is_64)
		width = 16;
	end = (char *)elf->addr + elf->size;
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
		len = bounded_strlen(symbols[i].name, end);
		write(1, symbols[i].name, len);
		write(1, "\n", 1);
		i++;
	}
}

void	sort_and_print_symbols(t_elf *elf, t_symbol *symbols, int count)
{
	sort_symbols(symbols, count);
	print_symbols(elf, symbols, count);
}

