#ifndef __ELF_H
#define __ELF_H

typedef unsigned int ELF32_Addr;
typedef unsigned short ELF32_Half;
typedef unsigned int ELF32_Off;
typedef unsigned int ELF32_Sword;
typedef unsigned int ELF32_Word;

#define EI_NIDENT 16

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  ELF32_Half e_type;
  ELF32_Half e_machine;
  ELF32_Word e_version;
  ELF32_Addr e_entry;  // entry point of program
  ELF32_Off e_phoff;   // program header table offset
  ELF32_Off e_shoff;   // section header talbe offset
  ELF32_Word e_flags;
  ELF32_Half e_hsize;  // ELF header size
  ELF32_Half e_phentsize; // program header entry size
  ELF32_Half e_phnum;     // number of entries in the program header
  ELF32_Half e_shentsize; // section header entry size
  ELF32_Half e_shnum;     // number of entries in the section header
  ELF32_Half e_shstrndx;  // string table index
}__attribute__((packed)) ELF32_Ehdr;

// Object file type
#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2
#define ET_DYN  3
#define ET_CORE 4

// Required architecture
#define EM_386 3

typedef struct {
  ELF32_Word sh_name;
  ELF32_Word sh_type;
  ELF32_Word sh_flags;
  ELF32_Addr sh_addr;
  ELF32_Off sh_offset;
  ELF32_Word sh_size;
  ELF32_Word sh_link;
  ELF32_Word sh_info;
  ELF32_Word sh_addralign;
  ELF32_Word sh_entsize;
}__attribute__((packed)) ELF32_Shdr;

// Section types
#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2

typedef struct {
  ELF32_Word st_name;
  ELF32_Addr st_value;
  ELF32_Word st_size;
  unsigned char st_info;
  unsigned char st_other;
  ELF32_Half st_shndx;
} ELF32_Sym;

#endif
