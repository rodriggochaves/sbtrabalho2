#include <iostream>
#include <fstream>
#include "gerador_elf.hpp"

GeradorElf::GeradorElf(std::string namefile) {
  this->file.open(namefile);
  if (!this->file.is_open()) {
    std::cout << "Error ao abrir o arquivo" << std::endl;
    exit(0);
  }
}

void GeradorElf::processFile() {
  std::string line;
  
  while( getline(this->file, line) ) {
    std::cout << line << std::endl;
  }
}