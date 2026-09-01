# Name of the output file
NAME = codexion


# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -I includes

	
# Source files
SRC = src/main.c \
		src/parser.c	

# Object files
OBJ = $(SRC:.c=.o)


# Default target
all: $(NAME)


# Rule to build the executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "✅ $(NAME) created successfully ✅"


# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Clean up object files and the executable
clean:
	rm -f $(OBJ)


# Clean up object files only
fclean: clean
	rm -f $(NAME)


# Rebuild everything from scratch
re: fclean all


# PHONY targets to avoid conflicts with files of the same name
.PHONY: all clean fclean re
