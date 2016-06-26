# compilador
CC = g++

# bibliotecas
LIBS = -std=c++11 -Wall

all: clean main

gerador_elf:
	$(CC) $(LIBS) -c gerador_elf.cpp

gerador: gerador_elf
	$(CC) $(LIBS) -c gerador.cpp
	$(CC) $(LIBS) -o gerador gerador.o gerador_elf.o
	./gerador ex1.asm

clean:
	rm -f gerador_elf.o
	rm -f gerador.o
	rm -f gerador
	rm -f output
	rm -f ex0.o
	rm -f ex0
	rm -f ex1.o
	rm -f ex1

exec: gerador
	chmod 755 output

ex0:
	nasm -f elf ex0.asm -o ex0.o
	ld -m elf_i386 -o ex0 ex0.o

ex1:
	nasm -f elf ex1.asm -o ex1.o
	ld -m elf_i386 -o ex1 ex1.o