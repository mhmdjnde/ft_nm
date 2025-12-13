/* ************************************************/
/*                                                */
/*                                                */
/*   extract_symbols.c                            */
/*                                                */
/*   By: JndeFromHome <jnde@student.jnde.fr>      */
/*                                                */
/*   Created: 2025/12/01 00:00:00 by JndeFromHome */
/*   Updated: 2025/12/01 00:00:00 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

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
					if (*name && ELF64_ST_TYPE(sym64->st_info) != STT_FILE)
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
					if (*name && ELF32_ST_TYPE(sym32->st_info) != STT_FILE)
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
					if (*name && ELF64_ST_TYPE(sym64->st_info) != STT_FILE)
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
					if (*name && ELF32_ST_TYPE(sym32->st_info) != STT_FILE)
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
