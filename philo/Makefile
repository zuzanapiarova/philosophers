CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -rf
AR = ar rcs
NAME = philo
HEADERS := -I ./include

SRCS := srcs/actions.c \
		srcs/actions_forks.c \
		srcs/input_val.c \
		srcs/logging.c \
		srcs/philo.c \
		srcs/routines.c \
		srcs/utils.c \
		srcs/num_utils.c \
		srcs/str_utils.c \

OBJS := ${SRCS:.c=.o}

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(HEADERS) -o $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) -o $@ -c $< $(HEADERS)

allc: all clean
	@echo "Making all and cleaning up"

clean:
	${RM} $(OBJS)

fclean: clean
	${RM} ${NAME}

re: fclean all clean
	@echo "Cleaning up and redoing all"


.PHONY: all clean fclean re allc
