NAME    = ft_nm

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g

SRC     = ft_nm.c elf_validation_and_ini.c find_symbol_tables.c
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
