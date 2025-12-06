NAME    = ft_nm

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g

SRC     = srcs/ft_nm.c srcs/elf_validation_and_ini.c srcs/find_symbol_tables.c srcs/symbol_type.c srcs/print_symbols.c srcs/extract_symbols.c
OBJ     = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
