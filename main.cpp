#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>
#include "gerador_elf.hpp"

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cout << "Número incorreto de argumentos" << std::endl;
    return 1;
  }

	GeradorElf geradorElf(argv[1]);

  geradorElf.readFile();

  // std::string data = { '\x48', '\x65', '\x6C', '\x6C', 
  // '\x6F','\x2C', '\x20', '\x57', '\x6F', '\x72', // “Hello, World!\n”
  // '\x6C', '\x64', '\x21', '\x0A' };

  geradorElf.createFile();
}