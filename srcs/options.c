/* ************************************************/
/*                                                */
/*                                                */
/*   options.c                                    */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

void	print_usage(void)
{
	ft_putstr_fd("Usage: ft_nm [option(s)] [file(s)]\n", 2);
	ft_putstr_fd(" List symbols in [file(s)] (a.out by default).\n", 2);
	ft_putstr_fd(" The options are:\n", 2);
	ft_putstr_fd("  -a   Display debugger-only symbols\n", 2);
	ft_putstr_fd("  -g   Display only external symbols\n", 2);
	ft_putstr_fd("  -p   Do not sort the symbols\n", 2);
	ft_putstr_fd("  -r   Reverse the sense of the sort\n", 2);
	ft_putstr_fd("  -u   Display only undefined symbols\n", 2);
}

int	set_option(t_opts *opts, char c)
{
	if (c == 'a')
		opts->all = 1;
	else if (c == 'g')
		opts->global = 1;
	else if (c == 'u')
		opts->undef = 1;
	else if (c == 'r')
		opts->reverse = 1;
	else if (c == 'p')
		opts->no_sort = 1;
	else
		return (0);
	return (1);
}

int	parse_options(int argc, char **argv, t_opts *opts, char **files)
{
	int	i;
	int	j;
	int	count;
	int	end_of_options;

	opts->all = 0;
	opts->global = 0;
	opts->undef = 0;
	opts->reverse = 0;
	opts->no_sort = 0;
	count = 0;
	end_of_options = 0;
	i = 1;
	while (i < argc)
	{
		//end of opts case when I use '--'
		if (!end_of_options && argv[i][0] == '-' && argv[i][1] == '-'
			&& argv[i][2] == '\0')
			end_of_options = 1;
		else if (!end_of_options && argv[i][0] == '-' && argv[i][1])
		{
			j = 1;
			while (argv[i][j])
			{
				if (!set_option(opts, argv[i][j]))
				{
					ft_putstr_fd("ft_nm: invalid option -- '", 2);
					write(2, &argv[i][j], 1);
					ft_putstr_fd("'\n", 2);
					print_usage();
					return (-1);
				}
				j++;
			}
		}
		else
		{
			files[count] = argv[i];
			count++;
		}
		i++;
	}
	files[count] = NULL;
	return (count);
}

int	filter_symbols(t_symbol *symbols, int count, t_opts *opts)
{
	int	i;
	int	kept;
	int	keep;

	i = 0;
	kept = 0;
	while (i < count)
	{
		if (opts->undef)
			keep = (symbols[i].section_index == SHN_UNDEF);
		else if (opts->global)
			keep = (symbols[i].bind == STB_GLOBAL
					|| symbols[i].bind == STB_WEAK
					|| symbols[i].bind == STB_GNU_UNIQUE
					|| symbols[i].section_index == SHN_UNDEF
					|| symbols[i].section_index == SHN_COMMON);
		else
			keep = 1;
		if (keep && !opts->all && (symbols[i].type == STT_FILE
				|| symbols[i].type == STT_SECTION))
			keep = 0;
		if (keep)
		{
			symbols[kept] = symbols[i];
			kept++;
		}
		i++;
	}
	return (kept);
}
