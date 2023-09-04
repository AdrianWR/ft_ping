# ft_ping

NAME = ft_ping

CC = clang

CFLAGS		= -Wall -Wextra -Werror
CPPFLAGS	= -I./include

LDFLAGS		=

SRC_DIR	= ./src
SRC			= $(shell find $(SRC_DIR) -type f -name "*.c")

OBJ_DIR	= ./build
OBJ 		= $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

INCLUDE_DIR = ./include
INCLUDE = $(shell find $(INCLUDE_DIR) -type f -name "*.h")

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
