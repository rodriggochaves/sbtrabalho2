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
	./main ex0.asm

clean:
	rm -f gerador_elf.o
	rm -f main.o
	rm -f main
	rm -f output
	rm -f ex0.o
	rm -f ex0
	rm -f ex1.o
	rm -f ex1

exec: main
	chmod 755 output

ex0:
	nasm -f elf ex0.asm -o ex0.o
	ld -m elf_i386 -o ex0 ex0.o

ex1:
	nasm -f elf ex1.asm -o ex1.o
	ld -m elf_i386 -o ex1 ex1.o