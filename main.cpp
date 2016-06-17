#include <iostream>
#include "gerador_elf.hpp"

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cout << "Número incorreto de argumentos" << std::endl;
    return 1;
  }

	GeradorElf geradorElf(argv[1]);
  geradorElf.createFile();
}