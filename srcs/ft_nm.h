/* ************************************************/
/*                                                */
/*                                                */
/*   ft_nm.h                                      */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#ifndef FT_NM_H
#define FT_NM_H

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef struct s_elf
{
	void	*addr;                 //base address of the mapped file in memory
	int		size;                  //total file size in bytes
	int		is_64;                 //1 if ELF64, 0 if ELF32
	int		section_header_offset; //offset in the file where the section header table starts (e_shoff)
	int		section_header_num;    //number of section headers (e_shnum)
	int		section_header_size;   //size of each section header entry (e_shentsize)
	int		section_str_index;     //index of the section-name string table (e_shstrndx)
	int		symtab_offset;         //offset of the main symbol table (.symtab) in the file
	int		symtab_num;            //number of entries in .symtab
	int		symtab_entsize;        //size of each symbol entry in .symtab
	int		symtab_str_offset;     //offset of the string table used by .symtab (.strtab)
	int		symtab_str_size;       //size (in bytes) of that .symtab string table
	int		dynsym_offset;         //offset of the dynamic symbol table (.dynsym) in the file
	int		dynsym_num;            //number of entries in .dynsym
	int		dynsym_entsize;        //size of each symbol entry in .dynsym
	int		dynsym_str_offset;     //offset of the string table used by .dynsym (.dynstr)
	int		dynsym_str_size;       //size (in bytes) of that .dynsym string table
}	t_elf;


typedef struct s_symbol
{
	char			*name;
	unsigned long	value;
	unsigned short	section_index;
	unsigned char	bind;
	unsigned char	type;
	int				is_dyn;
}	t_symbol;

int		find_symbol_tables(t_elf *elf);
int		validate_elf(void *addr, size_t size);
int		init_elf(void *addr, int size, t_elf *elf);
void	compute_symbol_types(t_elf *elf, t_symbol *symbols, int count, char *types);
int		extract_symbols(t_elf *elf, t_symbol **symbols, int *count);
void	sort_and_print_symbols(t_elf *elf, t_symbol *symbols, int count);
int		ft_strlen(char *str);
void	sort_symbols(t_symbol *symbols, int count);
void	print_symbols(t_elf *elf, t_symbol *symbols, int count);


#endif
