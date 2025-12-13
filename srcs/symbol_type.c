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

/*
** compute_symbol_types:
**   - For each symbol in the array `symbols` (size = count), computes
**     the nm-style type character (T/t, B/b, D/d, R/r, U, A, C, W/w, V/v, ?).
**   - The result for symbol i is stored in types[i].
**
**   Logic (simplified, but close to real nm):
**     1) If binding is STB_WEAK:
**          - If section_index == SHN_UNDEF:
**                * 'w' for weak function/normal symbol
**                * 'v' for weak object
**          - Else:
**                * 'W' for weak function/normal symbol
**                * 'V' for weak object
**     2) Else, based on section_index:
**          - SHN_UNDEF  -> 'U' (undefined)
**          - SHN_ABS    -> 'A' (absolute)
**          - SHN_COMMON -> 'C' (common)
**          - Otherwise, look at the section header:
**                * If SHT_NOBITS + SHF_ALLOC + SHF_WRITE -> 'B' (bss)
**                * Else if SHF_ALLOC + SHF_EXECINSTR    -> 'T' (text)
**                * Else if SHF_ALLOC + SHF_WRITE        -> 'D' (data)
**                * Else if SHF_ALLOC                    -> 'R' (rodata)
**                * Else                                 -> '?'
**     3) If binding is STB_LOCAL and the letter is an uppercase A–Z,
**        convert it to lowercase (t vs T, d vs D, b vs B, r vs R, etc.).
*/
void	compute_symbol_types(t_elf *elf, t_symbol *symbols, int count, char *types)
{
	int		i;
	char	*base;

	base = (char *)elf->addr;
	i = 0;
	while (i < count)
	{
		t_symbol		*sym;
		unsigned short	sec_index;
		char			c;

		sym = &symbols[i];
		sec_index = sym->section_index;
		c = '?';
		/* 1) Weak symbols: W/w or V/v */
		if (sym->bind == STB_WEAK)
		{
			if (sec_index == SHN_UNDEF)
			{
				if (sym->type == STT_OBJECT)
					c = 'v';
				else
					c = 'w';
			}
			else
			{
				if (sym->type == STT_OBJECT)
					c = 'V';
				else
					c = 'W';
			}
		}
		else if (sec_index == SHN_UNDEF)
			c = 'U';
		else if (sec_index == SHN_ABS)
			c = 'A';
		else if (sec_index == SHN_COMMON)
			c = 'C';
		else if (sec_index >= (unsigned short)elf->section_header_num)
			c = '?';
		else
		{
			int				hdr_off;
			unsigned long	flags;
			unsigned int	stype;

			hdr_off = elf->section_header_offset
				+ sec_index * elf->section_header_size;
			if (hdr_off < 0
				|| hdr_off + elf->section_header_size > elf->size)
				c = '?';
			else
			{
				if (elf->is_64)
				{
					Elf64_Shdr	*sh;

					sh = (Elf64_Shdr *)(base + hdr_off);
					flags = sh->sh_flags;
					stype = sh->sh_type;
				}
				else
				{
					Elf32_Shdr	*sh;

					sh = (Elf32_Shdr *)(base + hdr_off);
					flags = sh->sh_flags;
					stype = sh->sh_type;
				}
				if (stype == SHT_NOBITS
					&& (flags & SHF_ALLOC) && (flags & SHF_WRITE))
					c = 'B';
				else if ((flags & SHF_ALLOC) && (flags & SHF_EXECINSTR))
					c = 'T';
				else if ((flags & SHF_ALLOC) && (flags & SHF_WRITE))
					c = 'D';
				else if (flags & SHF_ALLOC)
					c = 'R';
				else
					c = '?';
			}
		}
		/* 3) Local symbols -> lowercase */
		if (sym->bind == STB_LOCAL && c >= 'A' && c <= 'Z')
			c = (char)(c - 'A' + 'a');
		types[i] = c;
		i++;
	}
}
