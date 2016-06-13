# compilador
CC = g++

# bibliotecas
LIBS = -std=c++11 -Wall

gerador_elf:
	$(CC) $(LIBS) -c gerador_elf.cpp

main: gerador_elf
	$(CC) $(LIBS) -c main.cpp
	$(CC) $(LIBS) -o main main.o gerador_elf.o

clean:
	rm -f gerador_elf.o
	rm -f main.o
	rm -f main