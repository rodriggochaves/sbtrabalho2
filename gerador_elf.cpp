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
    std::cout << "Erro ao abrir o arquivo" << std::endl;
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

std::string GeradorElf::assembleMOV(textNode node) {
  return "0";
}

bool GeradorElf::isString( dataNode node ) {
  return node.value[0] == '\'' ? true : false ;
}

dataNode GeradorElf::processDataNode( dataNode node ) {
  std::string complementString = "";
  int complement = 0;

  if (node.type == "dw" && !this->isString(node) ) {
    for (unsigned int i = 0; i < node.value.length(); ++i) {
      node.value[i] = this->convertToHex(node.value[i]);
    }
    complement = (4 - node.value.length()) / 2;
    while(complement != 0) {
      complementString += '\x0';
      complement--;
    }
    node.value = node.value + complementString;
  }

  if (node.type == "db" && !this->isString(node) ) {
    for (unsigned int i = 0; i < node.value.length(); ++i) {
      node.value[i] = this->convertToHex(node.value[i]);
    }
    complement = (2 - node.value.length()) / 2;
    while(complement != 0) {
      complementString += '\x0';
      complement--;
    }
    node.value = node.value + complementString;
  }

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
  
  node = this->processDataNode( node );

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

std::string GeradorElf::getOp2( std::string& line ) {
  std::string op = "";
  op = line.substr( 1, line.length() - 1 );
  line = "";
  return op;
}

std::string GeradorElf::getOp1( std::string& line ) {
  std::string op = "";
  int found = line.find( "," );
  int endLine = line.find( "\n" );

  if (found != -1) {
    op = line.substr(0,found);
    line = line.substr(found + 1, line.length());
  } else if ( op.empty() ) {
    op = line.substr( 0, endLine );
    line = "";
  }
  return op;
}

std::string GeradorElf::getInstruction( std::string& line ) {
  std::string instruction = "";
  int found = line.find( " " );

  if (found != -1) {
    instruction = line.substr(0,found);
    line = line.substr(found + 1, line.length());
  }
  return instruction;
}

std::string GeradorElf::getLabel( std::string& line ) {
  std::string label = "";
  int found = line.find( ":" );

  if (found != -1) {
    label = line.substr(0,found);
    line = line.substr(found + 1, line.length());
  }
  return label;
}

textNode GeradorElf::processTextLine(std::string line) {
  textNode node;
  std::string aux = "";
  std::string label = "";

  line = this->removeMultipleSpaces( line );

  label = this->getLabel( line );
  if( !line.empty() ) node.instruction = this->getInstruction( line );
  if( !line.empty() ) node.op1 = this->getOp1( line );
  if( !line.empty() ) node.op2 = this->getOp2( line );

  return node;
}

void GeradorElf::readFile() {
  std::string line;
  bool inSectionData = false;
  bool inSectionText = false;
  bool pause = true;
  dataNode datanode;
  textNode textnode;
  
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
      datanode = this->processDataLine( line );
      this->data += datanode.value;
    }
    if ( inSectionText && pause ) {
      textnode = this->processTextLine( line );
      // this->text += textnode.value;
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