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
#include <errno.h>
#include <locale.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

// Every offset and size that we take from the file is kept in a long (64 bits and
// signed), never in an int: a corrupted file can put a huge value in a 64-bit ELF
// field, and truncating it to an int would silently turn it into a small (or
// negative) number that would pass our checks and let us read outside the mapping.

typedef struct s_elf
{
	void	*addr;                 //base address of the mapped file in memory
	long	size;                  //total file size in bytes
	int		is_64;                 //1 if ELF64, 0 if ELF32
	long	section_header_offset; //offset in the file where the section header table starts (e_shoff)
	int		section_header_num;    //number of section headers (e_shnum)
	int		section_header_size;   //size of each section header entry (e_shentsize)
	int		section_str_index;     //index of the section-name string table (e_shstrndx)		those 3 for the [5] shstrtab to get the debug info for -a
	long	section_str_offset;    //offset of that section-name string table (.shstrtab)		those 3 for the [5] shstrtab to get the debug info for -a
	long	section_str_size;      //size (in bytes) of that section-name string table			those 3 for the [5] shstrtab to get the debug info for -a
	long	symtab_offset;         //offset of the main symbol table (.symtab) in the file
	int		symtab_num;            //number of entries in .symtab
	int		symtab_entsize;        //size of each symbol entry in .symtab
	long	symtab_str_offset;     //offset of the string table used by .symtab (.strtab)
	long	symtab_str_size;       //size (in bytes) of that .symtab string table
}	t_elf;

// One section header, already read out of the file. Elf32_Shdr and Elf64_Shdr hold
// the same fields with different widths, so we copy them once into this neutral
// struct and the rest of the program never has to know which class the file is.

typedef struct s_shdr
{
	char			*name;    //section name, taken from .shstrtab (NULL if unusable)
	unsigned int	type;     //sh_type
	unsigned long	flags;    //sh_flags
	long			offset;   //sh_offset
	long			size;     //sh_size
	unsigned int	link;     //sh_link
	long			entsize;  //sh_entsize
}	t_shdr;

typedef struct s_symbol
{
	char			*name;          //points into the mapping string name
	unsigned long	value;          //st_value: the address, or an alignment for a common
	unsigned short	section_index;  //st_shndx, or one of the special SHN_ values
	unsigned char	bind;           //high nibble of st_info: local, global, weak
	unsigned char	type;           //low nibble of st_info: func, object, section, file
}	t_symbol;

// The five options of the bonus part, one flag each.

typedef struct s_opts
{
	int	all;      //-a : also print the debugger-only symbols (file and section symbols)
	int	global;   //-g : print only the external (global or weak) symbols
	int	undef;    //-u : print only the undefined symbols
	int	reverse;  //-r : reverse the order of the sort
	int	no_sort;  //-p : do not sort, keep the order of the symbol table
}	t_opts;

int		in_file(t_elf *elf, long offset, long size);
char	*read_string(t_elf *elf, long str_offset, long str_size, long index);
int		read_section_header(t_elf *elf, int index, t_shdr *sh);
void	set_section_str_table(t_elf *elf);
int		validate_elf(void *addr, long size);
int		init_elf(void *addr, long size, t_elf *elf);
int		find_symbol_tables(t_elf *elf);
int		extract_symbols(t_elf *elf, t_symbol **symbols, int *count);
char	get_symbol_letter(t_elf *elf, t_symbol *sym);
void	sort_symbols(t_symbol *symbols, int count, int reverse);
void	print_symbols(t_elf *elf, t_symbol *symbols, int count);
int		parse_options(int argc, char **argv, t_opts *opts, char **files);
int		filter_symbols(t_symbol *symbols, int count, t_opts *opts);
int		ft_strlen(char *str);
void	ft_putstr_fd(char *s, int fd);
void	print_file_error(char *file, char *msg);

#endif
