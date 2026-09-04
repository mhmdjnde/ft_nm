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

int	compare_names(char *s1, char *s2, int reverse)
{
	if (reverse)
		return (strcoll(s2, s1));
	return (strcoll(s1, s2));
}

void	sort_symbols(t_symbol *symbols, int count, int reverse)
{
	int			i;
	int			j;
	t_symbol	tmp;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - 1 - i)
		{
			if (compare_names(symbols[j].name, symbols[j + 1].name,
					reverse) > 0)
			{
				tmp = symbols[j];
				symbols[j] = symbols[j + 1];
				symbols[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

void	put_hex_padded(unsigned long value, char *buf, int width)
{
	int	i;

	i = width - 1;
	while (i >= 0)
	{
		buf[i] = "0123456789abcdef"[value % 16];
		value = value / 16;
		i--;
	}
}

void	put_blank_column(char *buf, int width)
{
	int	i;

	i = width - 1;
	while (i >= 0)
	{
		buf[i] = ' ';
		i--;
	}
}

void	print_symbols(t_elf *elf, t_symbol *symbols, int count)
{
	char	head[20];
	int		width;
	int		i;
	char	c;

	width = 8;
	if (elf->is_64)
		width = 16;
	i = 0;
	while (i < count)
	{
		c = get_symbol_letter(elf, &symbols[i]);
		if (c == 'U' || c == 'w' || c == 'v')
			put_blank_column(head, width);
		else
			put_hex_padded(symbols[i].value, head, width);
		head[width] = ' ';
		head[width + 1] = c;
		head[width + 2] = ' ';
		write(1, head, width + 3);
		ft_putstr_fd(symbols[i].name, 1);
		write(1, "\n", 1);
		i++;
	}
}
