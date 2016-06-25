#include <fstream>
#include <typeinfo>
#include <elfio/elfio.hpp>
#include "gerador_elf.hpp"
#include <sstream>
#include <string>
#include <algorithm>

GeradorElf::GeradorElf(std::string namefile) {
  this->file.open(namefile);
  if (!this->file.is_open()) {
    std::cout << "Error ao abrir o arquivo" << std::endl;
    exit(0);
  }
}

char GeradorElf::convertToHex(char c) {
  switch(c) {
    case '0' : return '\x0';
    case '1' : return '\x1';
    case '2' : return '\x2';
    case '3' : return '\x3';
    case '4' : return '\x4';
    case '5' : return '\x5';
    case '6' : return '\x6';
    case '7' : return '\x7';
    case '8' : return '\x8';
    case '9' : return '\x9';
    case 'a' : return '\xA';
    case 'b' : return '\xB';
    case 'c' : return '\xC';
    case 'd' : return '\xD';
    case 'e' : return '\xE';
    case 'f' : return '\xF';
    default : return 0;
  }
}


std::string GeradorElf::processDataLine(std::string line) {
  std::string symbol;
  std::string type;
  std::string value;
  int counter = 0;

  for (unsigned int i = 0; i < line.length(); ++i) {
    if(line[i] == ' ' && counter < 2) {
      counter++;
      continue;
    }
    if (counter == 0) {
      symbol += line[i];
      continue;
    }
    if (counter == 1) {
      type += line[i];
      continue;
    }
    if (counter >= 2) {
      value += line[i];
      continue;
    }
  }

  // trata value
  if (value[0] == '\'') {
    int closeMark = value.find('\'', 1);
    std::string extraChars = value.substr(closeMark + 1, value.length());
    std::string extraChar;

    value = value.substr(0, closeMark);
    value.erase(std::remove(value.begin(), value.end(), '\''), value.end());

    for (unsigned int i = 0; i < extraChars.length(); ++i) {
      if (extraChars[i] == ' ') continue;
      if (extraChars[i] == 'h') {
        extraChar = this->convertToHex(extraChars[i - 2]);
        extraChar += this->convertToHex(extraChars[i - 1]);
      }
      if (extraChars[i] == '0' && extraChars[i+1] == 'x') {
        value += extraChars[i+1];
        value += extraChars[i+2];
      }
      value += extraChar;
      extraChar = "";
    }
  }
  return value;
}

bool Both_are_spaces(char lhs, char rhs) {
  return (lhs == rhs) && (lhs == ' '); 
}

// recebe uma linha e remove multiplos espaços consecutivos
std::string GeradorElf::removeMultipleSpaces(std::string line) {
  std::string newline;

  std::string::iterator new_end = std::unique(line.begin(), line.end(), 
    Both_are_spaces);
  line.erase(new_end, line.end());
  newline = line;
  
  // caso a linha comece com um espaço, esse espaço é removido.
  if (newline[0] == ' ') {
    newline.erase(0, 1);
  }

  return newline;
}

std::vector<std::string> GeradorElf::tokenize(std::string line) {
  std::vector<std::string> tokens;
  std::string aux = "";

  line = this->removeMultipleSpaces(line);

  std::cout << line << std::endl;

  for (unsigned int i = 0; i < line.length(); ++i) {
    if (line[i] != ' ' || (line[i] != ',' && line[i+1] != ' ')) {
      aux += line[i];
    } else {
      tokens.push_back(aux);
      aux = "";
    }
  }

  return tokens;
}

std::string GeradorElf::processTextLine(std::string line) {
  std::vector<std::string> tokens = this->tokenize(line);
  return "0";
}

void GeradorElf::readFile() {
  std::string line;
  bool inSectionData = false;
  bool inSectionText = false;
  bool pause = true;
  
  while( getline(this->file, line) ) {
    if (line == "section .data") {
      inSectionText = false;
      inSectionData = true;
      pause = false;
    }
    if (line == "section .text") {
      inSectionText = true;
      inSectionData = false; 
      pause = false;
    }
    if ( inSectionData && pause ) {
      this->data += this->processDataLine( line );
    }
    if ( inSectionText && pause ) {
      this->text += this->processTextLine( line );
    }
    pause = true;
  }
}

void GeradorElf::createFile() {
  ELFIO::elfio writer;

  writer.create( ELFCLASS32, ELFDATA2LSB );

  writer.set_os_abi( ELFOSABI_LINUX );
  writer.set_type( ET_EXEC );
  writer.set_machine( EM_386 );

  ELFIO::section* text_sec = writer.sections.add(".text");
  text_sec->set_type(SHT_PROGBITS);
  text_sec->set_flags( SHF_ALLOC | SHF_EXECINSTR );
  text_sec->set_addr_align( 0x10 );

  // text_sec->set_data( text, sizeof( text ) );
  text_sec->set_data( this->text );
  ELFIO::segment* text_seg = writer.segments.add();
  text_seg->set_type( PT_LOAD );
  text_seg->set_virtual_address( 0x08048000 );
  text_seg->set_physical_address( 0x08048000 );
  text_seg->set_flags( PF_X | PF_R );
  text_seg->set_align( 0x1000 );

  text_seg->add_section_index( text_sec->get_index(),
  text_sec->get_addr_align() );
  ELFIO::section* data_sec = writer.sections.add( ".data" );
  data_sec->set_type( SHT_PROGBITS );
  data_sec->set_flags( SHF_ALLOC | SHF_WRITE );
  data_sec->set_addr_align( 0x4 );
  
  // data_sec->set_data( data, sizeof( data ) );
  data_sec->set_data( this->data );
  ELFIO::segment* data_seg = writer.segments.add();
  data_seg->set_type( PT_LOAD );
  data_seg->set_virtual_address( 0x08048020 );
  data_seg->set_physical_address( 0x08048020 );
  data_seg->set_flags( PF_W | PF_R );
  data_seg->set_align( 0x10 );
  data_seg->add_section_index( data_sec->get_index(),
  data_sec->get_addr_align() );
  
  writer.set_entry( 0x08048000 );
  writer.save( "output" ); 
}