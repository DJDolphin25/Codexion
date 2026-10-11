# Name of the output file
NAME = codexion


# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I includes -g

	
# Source files
SRC = src/coders.c \
		src/dongles.c \
		src/scheduler_queue.c \
<<<<<<< HEAD
		src/scheduler_queue_heap.c \
		src/scheduler_queue_access.c \
		src/scheduler_queue_modify.c \
		src/scheduler_time.c \
		src/scheduler.c \
		src/logger.c \
		src/monitor.c \
		src/monitor_state.c \
		src/monitor_burnout.c \
		src/monitor_thread.c \
=======
		src/scheduler.c \
		src/logger.c \
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
		src/execute.c \
		src/main.c \
		src/mutex.c \
		src/parser.c \
		src/threads.c

<<<<<<< HEAD
=======
TEST_SRC = src/coders.c \
		src/dongles.c \
		src/scheduler_queue.c \
		src/scheduler.c \
		src/mutex.c \
		src/parser.c \
		src/tests.c
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f

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
	rm -f $(OBJ) $(TEST_OBJ)


# Clean up object files only
fclean: clean
	rm -f $(NAME)


# Rebuild everything from scratch
re: fclean all


# PHONY targets to avoid conflicts with files of the same name
.PHONY: all clean fclean re tests
