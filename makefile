# compilador
CC = g++

# bibliotecas
LIBS = -std=c++11 -Wall

all: clean main

gerador_elf:
	$(CC) $(LIBS) -c gerador_elf.cpp

main: gerador_elf
	$(CC) $(LIBS) -c main.cpp
	$(CC) $(LIBS) -o main main.o gerador_elf.o
	./main ex1.asm

clean:
	rm -f gerador_elf.o
	rm -f main.o
	rm -f main
	rm -f output