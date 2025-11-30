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
** find_symbol_tables:
**   - Walks the section header table described by t_elf.
**   - Looks for sections of type SHT_SYMTAB (regular symbols)
**     and SHT_DYNSYM (dynamic symbols).
**   - For each symbol table:
**       * checks that the table (sh_offset, sh_size) is inside the file.
**       * checks that sh_entsize matches the expected symbol entry size
**         (Elf64_Sym or Elf32_Sym).
**       * uses sh_link to locate the associated string table section
**         (must be of type SHT_STRTAB).
**       * checks that the string table (sh_offset, sh_size) is inside the file.
**   - Stores the offsets/sizes in the t_elf structure:
**       * symtab_*  for SHT_SYMTAB
**       * dynsym_*  for SHT_DYNSYM
**   - Returns 1 if at least one valid symbol table is found,
**     0 otherwise (and prints an error if nothing usable is found).
*/
//added by jnde
//sh_type is the section header type, here in this project
//we need to read the symbol table, so we need the SHT_SYMTAB and the SHT_DYNSYM
// and SHT_STRTAB that contains the symbol names
int	find_symbol_tables(t_elf *elf)
{
	char	*base;
	int		i;
	int		found;
	int		offset;
	char	*sh_ptr;

	base = (char *)elf->addr;
	found = 0;
	elf->symtab_offset = -1;
	elf->symtab_num = 0;
	elf->symtab_entsize = 0;
	elf->symtab_str_offset = -1;
	elf->symtab_str_size = 0;
	elf->dynsym_offset = -1;
	elf->dynsym_num = 0;
	elf->dynsym_entsize = 0;
	elf->dynsym_str_offset = -1;
	elf->dynsym_str_size = 0;
	i = 0;
	while (i < elf->section_header_num)
	{
		offset = elf->section_header_offset + i * elf->section_header_size;
		if (offset < 0 || offset + elf->section_header_size > elf->size)
		{
			write(2, "nm: section header out of file\n", 31);
			return (0);
		}
		sh_ptr = base + offset;
		if (elf->is_64)
		{
			Elf64_Shdr	*sh;
			int			type;
			int			sym_off;
			int			sym_size;
			int			sym_entsize;
			int			link;

			sh = (Elf64_Shdr *)sh_ptr;
			type = sh->sh_type;
			if (type == SHT_SYMTAB || type == SHT_DYNSYM)
			{
				sym_off = (int)sh->sh_offset;
				sym_size = (int)sh->sh_size;
				sym_entsize = sh->sh_entsize;
				link = sh->sh_link;
				if (sym_off < 0 || sym_off + sym_size > elf->size
					|| sym_entsize != (int)sizeof(Elf64_Sym)
					|| sym_entsize == 0)
				{
					write(2, "nm: invalid symbol table\n", 25);
				}
				else if (link < 0 || link >= elf->section_header_num)
					write(2, "nm: invalid string table index\n", 31);
				else
				{
					int			str_off;
					int			str_size;
					int			str_type;
					int			str_hdr_off;
					char		*str_hdr_ptr;
					Elf64_Shdr	*str_sh;

					str_hdr_off = elf->section_header_offset
						+ link * elf->section_header_size;
					if (str_hdr_off < 0
						|| str_hdr_off + elf->section_header_size > elf->size)
						write(2, "nm: string table header out of file\n", 36);
					else
					{
						str_hdr_ptr = base + str_hdr_off;
						str_sh = (Elf64_Shdr *)str_hdr_ptr;
						str_type = str_sh->sh_type;
						str_off = (int)str_sh->sh_offset;
						str_size = (int)str_sh->sh_size;
						if (str_type != SHT_STRTAB
							|| str_off < 0
							|| str_off + str_size > elf->size)
							write(2, "nm: invalid string table\n", 25);
						else
						{
							if (type == SHT_SYMTAB && elf->symtab_offset == -1)
							{
								elf->symtab_offset = sym_off;
								elf->symtab_num = sym_size / sym_entsize;
								elf->symtab_entsize = sym_entsize;
								elf->symtab_str_offset = str_off;
								elf->symtab_str_size = str_size;
								found = 1;
							}
							else if (type == SHT_DYNSYM
								&& elf->dynsym_offset == -1)
							{
								elf->dynsym_offset = sym_off;
								elf->dynsym_num = sym_size / sym_entsize;
								elf->dynsym_entsize = sym_entsize;
								elf->dynsym_str_offset = str_off;
								elf->dynsym_str_size = str_size;
								found = 1;
							}
						}
					}
				}
			}
		}
		else
		{
			Elf32_Shdr	*sh;
			int			type;
			int			sym_off;
			int			sym_size;
			int			sym_entsize;
			int			link;

			sh = (Elf32_Shdr *)sh_ptr;
			type = sh->sh_type;
			if (type == SHT_SYMTAB || type == SHT_DYNSYM)
			{
				sym_off = (int)sh->sh_offset;
				sym_size = (int)sh->sh_size;
				sym_entsize = sh->sh_entsize;
				link = sh->sh_link;
				if (sym_off < 0 || sym_off + sym_size > elf->size
					|| sym_entsize != (int)sizeof(Elf32_Sym)
					|| sym_entsize == 0)
				{
					write(2, "nm: invalid symbol table\n", 25);
				}
				else if (link < 0 || link >= elf->section_header_num)
					write(2, "nm: invalid string table index\n", 31);
				else
				{
					int			str_off;
					int			str_size;
					int			str_type;
					int			str_hdr_off;
					char		*str_hdr_ptr;
					Elf32_Shdr	*str_sh;

					str_hdr_off = elf->section_header_offset
						+ link * elf->section_header_size;
					if (str_hdr_off < 0
						|| str_hdr_off + elf->section_header_size > elf->size)
						write(2, "nm: string table header out of file\n", 36);
					else
					{
						str_hdr_ptr = base + str_hdr_off;
						str_sh = (Elf32_Shdr *)str_hdr_ptr;
						str_type = str_sh->sh_type;
						str_off = (int)str_sh->sh_offset;
						str_size = (int)str_sh->sh_size;
						if (str_type != SHT_STRTAB
							|| str_off < 0
							|| str_off + str_size > elf->size)
							write(2, "nm: invalid string table\n", 25);
						else
						{
							if (type == SHT_SYMTAB && elf->symtab_offset == -1)
							{
								elf->symtab_offset = sym_off;
								elf->symtab_num = sym_size / sym_entsize;
								elf->symtab_entsize = sym_entsize;
								elf->symtab_str_offset = str_off;
								elf->symtab_str_size = str_size;
								found = 1;
							}
							else if (type == SHT_DYNSYM
								&& elf->dynsym_offset == -1)
							{
								elf->dynsym_offset = sym_off;
								elf->dynsym_num = sym_size / sym_entsize;
								elf->dynsym_entsize = sym_entsize;
								elf->dynsym_str_offset = str_off;
								elf->dynsym_str_size = str_size;
								found = 1;
							}
						}
					}
				}
			}
		}
		i++;
	}
	if (!found)
	{
		write(2, "nm: no symbol table found\n", 26);
		return (0);
	}
	return (1);
}

