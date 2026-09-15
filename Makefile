NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread

SRCS		= src/main.c \
			  src/parsing.c \
			  src/init.c \
			  src/coder.c \
			  src/dongle.c \
			  src/scheduler.c \
			  src/monitor.c \
			  src/heap.c \
			  src/logger.c \
			  src/state.c \
			  src/time_utils.c

OBJS		= $(SRCS:.c=.o)
HEADER		= src/codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
