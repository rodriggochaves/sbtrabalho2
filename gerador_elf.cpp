#include <fstream>
#include <elfio/elfio.hpp>
#include "gerador_elf.hpp"

GeradorElf::GeradorElf(std::string namefile) {
  this->file.open(namefile);
  if (!this->file.is_open()) {
    std::cout << "Error ao abrir o arquivo" << std::endl;
    exit(0);
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

  char text[] = { '\xB8', '\x04', '\x00', '\x00', '\x00', // mov eax, 4
                  '\xBB', '\x01', '\x00', '\x00', '\x00', // mov ebx, 1
                  '\xB9', '\x20', '\x80', '\x04', '\x08', // mov ecx, msg
                  '\xBA', '\x0E', '\x00', '\x00', '\x00', // mov edx, 14
                  '\xCD', '\x80', // int 0x80
                  '\xB8', '\x01', '\x00', '\x00', '\x00', // mov eax, 1
                  '\xCD', '\x80' }; // int 0x80

  text_sec->set_data( text, sizeof( text ) );
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
  char data[] = { '\x48', '\x65', '\x6C', '\x6C', '\x6F', // “Hello, World!\n”
  '\x2C', '\x20', '\x57', '\x6F', '\x72',
  '\x6C', '\x64', '\x21', '\x0A' };
  data_sec->set_data( data, sizeof( data ) );

  ELFIO::segment* data_seg = writer.segments.add();
  data_seg->set_type( PT_LOAD );
  data_seg->set_virtual_address( 0x08048020 );
  data_seg->set_physical_address( 0x08048020 );
  data_seg->set_flags( PF_W | PF_R );
  data_seg->set_align( 0x10 );
  data_seg->add_section_index( data_sec->get_index(),
  data_sec->get_addr_align() );
  
  writer.set_entry( 0x08048000 );
  writer.save( "hello_i386_32" ); 
}

void GeradorElf::processFile() {
  std::string line;
  
  while( getline(this->file, line) ) {
    std::cout << line << std::endl;
  }
}