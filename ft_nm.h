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
	void	*addr;
	int		size;
	int		is_64;
	int		section_header_offset;
	int		section_header_num;
	int		section_header_size;
	int		section_str_index;
	int  symtab_offset;
	int  symtab_num;
	int  symtab_entsize;
	int  symtab_str_offset;
	int  symtab_str_size;
	int  dynsym_offset;
	int  dynsym_num;
	int  dynsym_entsize;
	int  dynsym_str_offset;
	int  dynsym_str_size;
}	t_elf;

int	find_symbol_tables(t_elf *elf);
int	validate_elf(void *addr, size_t size);
int	init_elf(void *addr, int size, t_elf *elf);


#endif
