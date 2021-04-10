# My first makefile

NAME=dango
CC=gcc
DEBUG_FLAGS= -g -fsanitize=address
SRC=src/*.c

all: $(SRC)
	$(CC) -o $(NAME) $(SRC) $(DEBUG_FLAGS)

clean:
	rm -f *.o
