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
  }
}

char GeradorElf::convertToHex(char c) {
  switch(c) {
    case '1' : return '\x1';
    case '2' : return '\x2';
    case '3' : return '\x3';
    case '4' : return '\x4';
    case '5' : return '\x5';
    case '6' : return '\x6';
    case '7' : return '\x7';
    // case '8' : return '\x8';
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

std::string GeradorElf::assembleMOV(std::vector<std::string> tokens) {
  return "0";
}

bool GeradorElf::isString( dataNode node ) {
  return node.value[0] == '\'' ? true : false ;
}

dataNode GeradorElf::processDataNode( dataNode node ) {
  std::string complementString = "";
  int complement = 0;

  if (node.type == "dd" && !this->isString(node) ) {
    for (unsigned int i = 0; i < node.value.length(); ++i) {
      node.value[i] = this->convertToHex(node.value[i]);
    }
    complement = 8 - node.value.length();
    while(complement != 0) {
      complementString += '\x0';
      complement--;
    }
    node.value = complementString + node.value;
  }

  // std::cout << node.value << std::endl;

  return node;
}

dataNode GeradorElf::processDataLine(std::string line) {
  dataNode node;
  int counter = 0;

  for (unsigned int i = 0; i < line.length(); ++i) {
    if(line[i] == ' ' && counter < 2) {
      counter++;
      continue;
    }
    if (counter == 0) {
      node.symbol += line[i];
      continue;
    }
    if (counter == 1) {
      node.type += line[i];
      continue;
    }
    if (counter >= 2) {
      node.value += line[i];
      continue;
    }
  }

  // std::cout << node.symbol << std::endl;
  // std::cout << node.type << std::endl;
  // std::cout << node.value << std::endl;
  
  node = this->processDataNode(node);

  return node;
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

  // std::cout << line << std::endl;

  for (unsigned int i = 0; i < line.length(); ++i) {
    if ( line[i] == ' ' ) {
      tokens.push_back(aux);
      aux = "";
    } else if ( line[i] == ',' && line[i + 1] == ' ' ) {
      tokens.push_back(aux);
      aux = "";
      i = i + 1;
    } else {
      aux += line[i];
    }
  }
  if (!aux.empty()) tokens.push_back(aux);
  return tokens;
}

std::string GeradorElf::processTextLine(std::string line) {
  std::vector<std::string> tokens = this->tokenize(line);
  std::string code = "";
  
  if (tokens[0] == "global") return  "";
  if (tokens[0] == "mov") code = this->assembleMOV(tokens);

  return "0";
}

void GeradorElf::readFile() {
  std::string line;
  bool inSectionData = false;
  bool inSectionText = false;
  bool pause = true;
  dataNode node;
  
  while( getline(this->file, line) ) {
    if ( line.empty() ) {
      continue;
    }
    if ( line == "section .data" ) {
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
      node = this->processDataLine( line );
      this->data += node.value;
    }
    if ( inSectionText && pause ) {
      // this->text += this->processTextLine( line );
    }
    pause = true;
  }
}

void GeradorElf::createFile() {
  std::string text = { '\xB8', '\x04', '\x00', '\x00', 
                  '\x00', // mov eax, 4
                  '\xBB', '\x01', '\x00', '\x00', '\x00', // mov ebx, 1
                  '\xB9', '\x20', '\x80', '\x04', '\x08', // mov ecx, msg
                  '\xBA', '\x0E', '\x00', '\x00', '\x00', // mov edx, 14
                  '\xCD', '\x80', // int 0x80
                  '\xB8', '\x01', '\x00', '\x00', '\x00', // mov eax, 1
                  '\xCD', '\x80' }; // int 0x80

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
  text_sec->set_data( text );
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