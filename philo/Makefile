CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -rf
AR = ar rcs
NAME = philo
HEADERS := -I ./include
SAN = 
#-fsanitize=thread

SRCS := srcs/philo.c \
		srcs/init.c \
		srcs/routines.c \
		srcs/actions.c \
		srcs/actions_forks.c \
		srcs/cleanup.c \
		srcs/utils/input_val.c \
		srcs/utils/logging.c \
		srcs/utils/num_utils.c \
		srcs/utils/str_utils.c \
		srcs/utils/utils.c \

OBJS := ${SRCS:.c=.o}

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(HEADERS) $(SAN) -o $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) $(SAN) -o $@ -c $< $(HEADERS)

allc: all clean
	@echo "Making all and cleaning up"

clean:
	${RM} $(OBJS)

fclean: clean
	${RM} ${NAME}

re: fclean all clean
	@echo "Cleaning up and redoing all"


.PHONY: all clean fclean re allc
