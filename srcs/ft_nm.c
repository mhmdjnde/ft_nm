/* ************************************************/
/*                                                */
/*                                                */
/*   ft_nm.c                                      */
/*                                                */
/*   By: JndeFromHome                             */
/*                                                */
/*   Created: 2025/11/30 23:33:56 by JndeFromHome */
/*   Updated: 2029/11/99 23:33:56 by JndeFromHome */
/*                                                */
/* ************************************************/

#include "ft_nm.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_putstr_fd(char *s, int fd)
{
	if (!s)
		return ;
	write(fd, s, ft_strlen(s));
}

void	print_file_error(char *file, char *msg)
{
	ft_putstr_fd("ft_nm: ", 2);
	ft_putstr_fd(file, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(msg, 2);
	ft_putstr_fd("\n", 2);
}

void	*map_file(int fd, char *file, long *size)
{
	struct stat	st;
	void		*addr;

	if (fstat(fd, &st) < 0)
	{
		print_file_error(file, "could not be read");
		return (NULL);
	}
	if (S_ISDIR(st.st_mode))
	{
		ft_putstr_fd("ft_nm: Warning: '", 2);
		ft_putstr_fd(file, 2);
		ft_putstr_fd("' is a directory\n", 2);
		return (NULL);
	}
	if (st.st_size < 1)
		return (NULL);
	addr = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (addr == MAP_FAILED)
	{
		print_file_error(file, "file format not recognized");
		return (NULL);
	}
	*size = (long)st.st_size;
	return (addr);
}

int	display_elf(void *addr, long size, char *file, t_opts *opts,
		int show_name)
{
	t_elf		elf;
	t_symbol	*symbols;
	int			count;

	if (!validate_elf(addr, size) || !init_elf(addr, size, &elf)
		|| !find_symbol_tables(&elf))
	{
		print_file_error(file, "file format not recognized");
		return (0);
	}
	if (show_name)
	{
		write(1, "\n", 1);
		ft_putstr_fd(file, 1);
		write(1, ":\n", 2);
	}
	if (!extract_symbols(&elf, &symbols, &count))
	{
		print_file_error(file, "out of memory");
		return (0);
	}
	if (count == 0)
	{
		free(symbols);
		print_file_error(file, "no symbols");
		return (1);
	}
	count = filter_symbols(symbols, count, opts);
	if (!opts->no_sort)
		sort_symbols(symbols, count, opts->reverse);
	print_symbols(&elf, symbols, count);
	free(symbols);
	return (1);
}

int	display_file(char *file, t_opts *opts, int show_name)
{
	int		fd;
	void	*addr;
	long	size;
	int		ok;

	fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		if (errno == ENOENT)
		{
			ft_putstr_fd("ft_nm: '", 2);
			ft_putstr_fd(file, 2);
			ft_putstr_fd("': No such file\n", 2);
		}
		else if (errno == EACCES)
			print_file_error(file, "Permission denied");
		else
			print_file_error(file, "could not be read");
		return (0);
	}
	size = 0;
	addr = map_file(fd, file, &size);
	close(fd);
	if (!addr)
		return (0);
	ok = display_elf(addr, size, file, opts, show_name);
	munmap(addr, size);
	return (ok);
}

int	main(int argc, char **argv)
{
	t_opts	opts;
	char	**files;
	int		count;
	int		i;
	int		status;

	setlocale(LC_COLLATE, "");	//set the lang to the machine's lang, used for strcoll later
	files = (char **)malloc((argc + 2) * sizeof(char *));
	if (!files)
		return (1);
	count = parse_options(argc, argv, &opts, files);
	if (count < 0)
	{
		free(files);
		return (1);
	}
	if (count == 0)
	{
		files[0] = "a.out";
		files[1] = NULL;
		count = 1;
	}
	status = 0;
	i = 0;
	while (i < count)
	{
		if (!display_file(files[i], &opts, count > 1))
			status = 1;
		i++;
	}
	free(files);
	return (status);
}
