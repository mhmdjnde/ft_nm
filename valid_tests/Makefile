CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g

SRCS    = test1_basic.c \
          test2_bss.c \
          test3_rodata.c \
          test4_static_funcs.c \
          test5_weak.c \
          test6_extern.c \
          test7_multiple.c \
          test8_struct.c \
          test9_static_data.c \
          test10_funcptr.c

OBJS    = $(SRCS:.c=.o)

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
