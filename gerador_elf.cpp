#include <fstream>
#include <typeinfo>
#include <elfio/elfio.hpp>
#include "gerador_elf.hpp"
#include <sstream>
#include <string>
#include <algorithm>
#include <stdint.h>
#include <iomanip>

GeradorElf::GeradorElf(std::string namefile) {
  this->file.open(namefile);
  if (!this->file.is_open()) {
  }
  this->currentDataPosition = 0x08049098;
  this->currentTextPosition = 0x08048080;
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

std::string GeradorElf::undercase( std::string data ) {
  std::transform(data.begin(), data.end(), data.begin(), ::tolower);
  return data;
}

bool GeradorElf::isRegister ( std::string name ) {
  if (name == "eax" || name == "ebx" || name == "ecx" || name == "edx" ||
    name == "esp" || name == "ebp" || name == "ebi" || name == "esi") {
    return true;
  } else {
    return false;
  }
}

bool GeradorElf::isMemory ( std::string name ) {
  return (name.front() == '[' && name.back() == ']') ? true : false;
}

bool GeradorElf::isImmediate ( std::string name ) {
  return (!this->isRegister(name) && !this->isMemory(name)) ? true : false;
}

std::string GeradorElf::filterMemory( std::string op ) {
  std::string newOp = "";

  for (unsigned int i = 0; i < op.length(); ++i) {
    if (op[i] != '[' && op[i] != ']' && op[i] != ' ') {
      newOp += op[i];
    }
  }
  return newOp;
}

dataNode GeradorElf::findSymbol( std::string op ) {
  dataNode node;
  for (auto symbol : this->symbols) {
    if (op == symbol.symbol) {
      node = symbol;
      break;
    }
  }
  return node;
}

long long int GeradorElf::reverseNumber ( long long int number ) {
  long long int reverse = 0;
  long long int aux = 0;
  int i = sizeof(number) / 2;

  while(i != 0) {
    aux = number % 0x100LL;
    reverse = (reverse) | aux;
    reverse = reverse << 2 * 4;
    number /= 0x100LL;
    i--;
  }
  reverse = reverse >> 2 * 4;

  return reverse;
}

long long int GeradorElf::getRegistersNumber ( std::string reg ) {
  reg = this->undercase(reg);
  if( reg =="eax" ) return 0x0;
  if( reg =="ecx" ) return 0x1;
  if( reg =="edx" ) return 0x2;
  if( reg =="ebx" ) return 0x3;
  if( reg =="esi" ) return 0x6;
  if( reg =="edi" ) return 0x7;
  return 0x0;
}

long long int GeradorElf::hexParamater( std::string op ) {
  long long int number;
  op = this->undercase( op );
  unsigned int length = op.length();

  if (op[length - 1] == 'h')  {
    op.pop_back();
    std::istringstream iss(op);
    iss >> std::hex >> number;
  }
  if (op[0] == '0' && op[1] == 'x') {
    std::string aux = op.substr(2, length);
    std::istringstream iss(aux);
    iss >> std::hex >> number; 
  }
  return number;
}

int GeradorElf::numberOfDigits( long long int number, int base ) {
 int number_of_digits = 0;

  do {
       ++number_of_digits; 
       number /= base;;
  } while (number);

  return number_of_digits;
}

void GeradorElf::assemble(textNode& node) {
  dataNode memoryAccess;

  node.code = 0x0;
  node.valid = false;

  if (this->undercase(node.instruction) == "mov") {
    if (isRegister(node.op1) && isMemory(node.op2)) {
      node.op2 = this->filterMemory( node.op2 );
      memoryAccess = this->findSymbol( node.op2 );
      if (node.op1 == "eax") {
        int size = sizeof(memoryAccess.position);
        node.code = (0xA1LL << size * 4) | 
          this->reverseNumber(memoryAccess.position);
      }
    } else if (isRegister(node.op1) && isImmediate(node.op2)) {
      long long int immediate = 0x0;
      node.code = 0xB8LL + this->getRegistersNumber(node.op1);
      std::istringstream iss(node.op2);
      iss >> std::hex >> immediate;
      int size = sizeof(immediate);
      node.code = (node.code << size * 4) | this->reverseNumber(immediate);
    }
    node.valid = true;
  }

  if (this->undercase(node.instruction) == "add") {
    if (isRegister(node.op1) && isMemory(node.op2)) {
      node.op2 = this->filterMemory( node.op2 );
      if (!this->isRegister(node.op2)) 
        memoryAccess = this->findSymbol( node.op2 );;
      // TODO else caso tenhamos um registrador com ponteiro para a memoria
      if (node.op1 == "eax") {
        int size = sizeof(memoryAccess.position);
        node.code = (0x0305LL << size * 4) 
          | this->reverseNumber(memoryAccess.position);
      }
    }
    node.valid = true;
  }

  if (this->undercase(node.instruction) == "int") {
    long long int code = this->hexParamater( node.op1 );
    int size = this->numberOfDigits(code, 16);
    node.code = (0xCDLL << size * 4) | code;
    node.valid = true;
  }

  if ( node.valid ) {
    int count = this->numberOfDigits(node.code, 16) / 2;
    if (this->numberOfDigits(node.code, 16) % 2 != 0) count++;
    this->currentTextPosition += count;
    node.label = this->currentLabel;
  }
}

bool GeradorElf::isString( dataNode node ) {
  return node.value[0] == '\'' ? true : false ;
}

dataNode GeradorElf::processDataNode( dataNode node ) {
  std::string complementString = "";
  int complement = 0;
  int times = std::stoi(node.times);

  if (node.type == "dd" && !this->isString(node) ) {
    for (unsigned int i = 0; i < node.value.length(); ++i) {
      node.value[i] = this->convertToHex(node.value[i]);
    }
    complement = (8 - node.value.length()) / 2;
    while(complement != 0) {
      complementString += '\x0';
      complement--;
    }
    node.value = node.value + complementString;
    node.position = this->currentDataPosition * times;
    currentDataPosition += times * 8 / 2;
  }

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
    node.position = this->currentDataPosition * times;
    currentDataPosition += times * 4 / 2;
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
    node.position = this->currentDataPosition * times;
    currentDataPosition += times * 2 / 2;
  }

  return node;
}

std::string GeradorElf::getToken( std::string& line ) {
  std::string token;
  int found = line.find(" ");
  int length = line.length();
  token = line.substr(0, found);
  line = line.substr(found + 1, length - 1);

  return token;
}

dataNode GeradorElf::processDataLine(std::string line) {
  dataNode node;

  line = this->removeMultipleSpaces( line );

  node.symbol = this->getToken( line );
  if( !line.empty() ) node.type = this->getToken( line );
  if( !line.empty() ) node.value = this->getToken( line );
  if( !line.empty() && node.type == "times") {
    node.times = node.value;
    node.type = this->getToken( line );
    node.value = this->getToken( line );
  } else {
    node.times = "1";
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

void GeradorElf::storeLabel( std::string line ) {
  labelNode node;

  node.label = line;
  node.address = this->currentTextPosition;

  this->labels.push_back( node );

  this->currentLabel = line;
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
  else if ( !label.empty() ) this->storeLabel( label );
  if( !line.empty() ) node.op1 = this->getOp1( line );
  if( !line.empty() ) node.op2 = this->getOp2( line );

  this->assemble( node );

  return node;
}

void GeradorElf::readFile() {
  std::string line;
  bool allowRead = false;
  dataNode datanode;
  textNode textnode;

  // vamos focar em achar a section data
  while ( getline( this->file, line ) ) {
    if ( line == "section .data" ) {
      allowRead = true;
      continue;
    }
    if ( line.empty() ) allowRead = false;
    if ( allowRead ) {
      this->symbols.push_back(this->processDataLine( line ));
    }
  }

  // resetamos o ponteiro do arquivo para o inicio
  this->file.clear();
  this->file.seekg(0, this->file.beg);
  
  // agora podemos ler as instruções e ignorar a sessão data
  while( getline(this->file, line) ) {
    
    if ( line == "section .data" ) allowRead = false;
    if ( line.empty() ) allowRead = true;
    if ( line == "section .text" ) {
      allowRead = true;
      continue;
    }
    if ( allowRead )
      this->instructions.push_back(this->processTextLine( line ));
  }
}

std::string GeradorElf::convertInstructions( std::string text ) {
  std::string newText = "";

  for (unsigned int i = 0; i < text.length(); i+=2) {
    std::string byte = text.substr(i,2);
    char chr = (char) (int) strtol( byte.c_str(), NULL, 16 );
    newText.push_back(chr);
  }

  return newText;
}

void GeradorElf::createFile() {
  std::string data = "";
  for (auto dNode : this->symbols) {
    int times = std::stoi( dNode.times );
    for (int i = 0; i < times; i++ ) data += dNode.value;
  };

  ELFIO::elfio writer;

  writer.create( ELFCLASS32, ELFDATA2LSB );

  writer.set_os_abi( ELFOSABI_LINUX );
  writer.set_type( ET_EXEC );
  writer.set_machine( EM_386 );

  for ( auto label : this->labels ) {
    std::string text = "";
    std::string textResult = "";
    for ( auto i : this->instructions ) {
      if ( i.valid && i.label == label.label) {
        std::stringstream stream;
        stream << std::hex << i.code;
        std::string result( stream.str() );
        if (result.length() % 2 != 0) result.insert(0, "0");
        text += result;
      }
    };
    textResult = this->convertInstructions( text );
    
    ELFIO::section* text_sec = writer.sections.add(label.label);
    text_sec->set_type(SHT_PROGBITS);
    text_sec->set_flags( SHF_ALLOC | SHF_EXECINSTR );
    text_sec->set_addr_align( 0x10 );

    // text_sec->set_data( text, sizeof( text ) );
    text_sec->set_data( textResult );
    ELFIO::segment* text_seg = writer.segments.add();
    text_seg->set_type( PT_LOAD );
    text_seg->set_virtual_address( label.address );
    text_seg->set_physical_address( label.address );
    text_seg->set_flags( PF_X | PF_R );
    text_seg->set_align( 0x1000 );
    text_seg->add_section_index( text_sec->get_index(),
      text_sec->get_addr_align() );    
  }  

  // ELFIO::section* text_sec = writer.sections.add("_text");
  // text_sec->set_type(SHT_PROGBITS);
  // text_sec->set_flags( SHF_ALLOC | SHF_EXECINSTR );
  // text_sec->set_addr_align( 0x10 );

  // // text_sec->set_data( text, sizeof( text ) );
  // text_sec->set_data( textResult );
  // ELFIO::segment* text_seg = writer.segments.add();
  // text_seg->set_type( PT_LOAD );
  // text_seg->set_virtual_address( 0x08048080 );
  // text_seg->set_physical_address( 0x08048080 );
  // text_seg->set_flags( PF_X | PF_R );
  // text_seg->set_align( 0x1000 );
  // text_seg->add_section_index( text_sec->get_index(),
  //   text_sec->get_addr_align() );


  ELFIO::section* data_sec = writer.sections.add( ".data" );
  data_sec->set_type( SHT_PROGBITS );
  data_sec->set_flags( SHF_ALLOC | SHF_WRITE );
  data_sec->set_addr_align( 0x4 );
  
  // data_sec->set_data( data, sizeof( data ) );
  data_sec->set_data( data );
  ELFIO::segment* data_seg = writer.segments.add();
  data_seg->set_type( PT_LOAD );
  data_seg->set_virtual_address( 0x08049098 );
  data_seg->set_physical_address( 0x08049098 );
  data_seg->set_flags( PF_W | PF_R );
  data_seg->set_align( 0x10 );
  data_seg->add_section_index( data_sec->get_index(),
    data_sec->get_addr_align() );
  
  writer.set_entry( 0x08048080 );
  writer.save( "output" ); 
}