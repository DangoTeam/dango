# My first makefile

NAME=dango
CC=g++
DEBUG_FLAGS= -g -fsanitize=address
SRC=src/*.cpp
PREFIX = /usr/local

dango: $(SRC)
	$(CC) -o $(NAME) $(SRC) $(DEBUG_FLAGS) 

install: dango
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dango ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dango
clean:
	rm -f *.o
