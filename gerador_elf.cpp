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

void GeradorElf::createFile(std::vector<char> text, char* data) {
  std::cout << sizeof( text ) << std::endl;

  // ELFIO::elfio writer;

  // writer.create( ELFCLASS32, ELFDATA2LSB );

  // writer.set_os_abi( ELFOSABI_LINUX );
  // writer.set_type( ET_EXEC );
  // writer.set_machine( EM_386 );

  // ELFIO::section* text_sec = writer.sections.add(".text");
  // text_sec->set_type(SHT_PROGBITS);
  // text_sec->set_flags( SHF_ALLOC | SHF_EXECINSTR );
  // text_sec->set_addr_align( 0x10 );

  // text_sec->set_data( text, sizeof( text ) );
  // ELFIO::segment* text_seg = writer.segments.add();
  // text_seg->set_type( PT_LOAD );
  // text_seg->set_virtual_address( 0x08048000 );
  // text_seg->set_physical_address( 0x08048000 );
  // text_seg->set_flags( PF_X | PF_R );
  // text_seg->set_align( 0x1000 );

  // text_seg->add_section_index( text_sec->get_index(),
  // text_sec->get_addr_align() );
  // ELFIO::section* data_sec = writer.sections.add( ".data" );
  // data_sec->set_type( SHT_PROGBITS );
  // data_sec->set_flags( SHF_ALLOC | SHF_WRITE );
  // data_sec->set_addr_align( 0x4 );
  
  // data_sec->set_data( data, sizeof( data ) );

  // ELFIO::segment* data_seg = writer.segments.add();
  // data_seg->set_type( PT_LOAD );
  // data_seg->set_virtual_address( 0x08048020 );
  // data_seg->set_physical_address( 0x08048020 );
  // data_seg->set_flags( PF_W | PF_R );
  // data_seg->set_align( 0x10 );
  // data_seg->add_section_index( data_sec->get_index(),
  // data_sec->get_addr_align() );
  
  // writer.set_entry( 0x08048000 );
  // writer.save( "output" ); 
}

void GeradorElf::processFile() {
  std::string line;
  
  while( getline(this->file, line) ) {
    std::cout << line << std::endl;
  }
}