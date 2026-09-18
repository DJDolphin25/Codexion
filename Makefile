# Name of the output file
NAME = codexion
TEST_NAME = tests


# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I includes -g

	
# Source files
SRC = src/coders.c \
		src/dongles.c \
		src/execute.c \
		src/main.c \
		src/mutex.c \
		src/parser.c \
		src/threads.c

TEST_SRC = src/coders.c \
		src/dongles.c \
		src/mutex.c \
		src/parser.c \
		src/tests.c

# Object files
OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)


# Default target
all: $(NAME)


# Test executable
tests: $(TEST_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) -o $(TEST_NAME)
	@echo "✅ $(TEST_NAME) created successfully ✅"


# Rule to build the executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "✅ $(NAME) created successfully ✅"


# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Clean up object files and the executable
clean:
	rm -f $(OBJ) $(TEST_OBJ)


# Clean up object files only
fclean: clean
	rm -f $(NAME)
	rm -f $(TEST_NAME)


# Rebuild everything from scratch
re: fclean all


# PHONY targets to avoid conflicts with files of the same name
.PHONY: all clean fclean re tests
