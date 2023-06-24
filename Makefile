# ft_ping

NAME = ft_ping

CC = gcc

CFLAGS		= -Wall -Wextra -Werror
CPPFLAGS	= -I./include

LDFLAGS		=

SRC_DIR	= ./src
SRC			= $(shell find $(SRC_DIR) -type f -name "*.c")

OBJ_DIR	= ./build
OBJ 		= $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re