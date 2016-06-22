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

	std::string text = { '\xB8', '\x04', '\x00', '\x00', 
									'\x00', // mov eax, 4
                  '\xBB', '\x01', '\x00', '\x00', '\x00', // mov ebx, 1
                  '\xB9', '\x20', '\x80', '\x04', '\x08', // mov ecx, msg
                  '\xBA', '\x0E', '\x00', '\x00', '\x00', // mov edx, 14
                  '\xCD', '\x80', // int 0x80
                  '\xB8', '\x01', '\x00', '\x00', '\x00', // mov eax, 1
                  '\xCD', '\x80' }; // int 0x80

  // std::string data = { '\x48', '\x65', '\x6C', '\x6C', 
  // '\x6F','\x2C', '\x20', '\x57', '\x6F', '\x72', // “Hello, World!\n”
  // '\x6C', '\x64', '\x21', '\x0A' };

  // std::cout << data << std::endl;

  geradorElf.createFile(text);
}