//#include <kb.h>
#include <str.h>
#include <system.h>
#include <fs.h>
#include <elf.h>

#include <ls.h>
#include <cat.h>

void objdump(char *path) {
  file_t *f = fopen(path, 'r');
  if (!f) {
    printf("File not found\n");
    return;
  }
  fseek(f, 0, SEEK_END);
  int file_len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *contents = (char*)malloc(file_len);
  fread(contents, file_len, 1, f);

  ELF32_Ehdr *header = (ELF32_Ehdr*)malloc(sizeof(ELF32_Ehdr));
  memcpy(header, contents, sizeof(ELF32_Ehdr));
  printf("Entry point: 0x%x, type: %d\n", header->e_entry, header->e_type);
  printf("Flags: %b\n", header->e_flags);

  ELF32_Shdr *s_strings = (ELF32_Shdr*)malloc(sizeof(ELF32_Shdr));
  memcpy(s_strings, contents + header->e_shoff + header->e_shstrndx*header->e_shentsize, sizeof(ELF32_Shdr));

  int i;
  printf("String entries at 0x%x\n", header->e_shstrndx);
  printf("%d program entries at 0x%x\n", header->e_phnum, header->e_phoff);
  printf("%d section entries, size %d at 0x%x\n", header->e_shnum, header->e_shentsize, header->e_shoff);
  ELF32_Shdr *s_header = (ELF32_Shdr*)malloc(sizeof(ELF32_Shdr));
  for (i = 0;; i++) {
    if (i > 8)  break;
    memcpy(s_header, contents + header->e_shoff + i*header->e_shentsize, sizeof(ELF32_Shdr));
    if (s_header->sh_type == SHT_NULL)
      continue;
    printf("%s: 0x%x\n",
           contents + s_strings->sh_offset + s_header->sh_name,
           s_header->sh_offset);
  }
}

unsigned int sh() {
  char c, line[80], cmd[80], arg[80];
  int i;
  printf("Basic shell v0.1\n");

  objdump("/tests/syscalls");
  while(1) {
    printf("$ ");
    i = 0;
    memset(line, '\0', 80);
    memset(cmd, '\0', 80);
    memset(arg, '\0', 80);
    while((c = getch()) != '\n') {
      if (c == '\b') {
        if (i == 0) continue;
        i--;
      } else {
        line[i] = c;
        i++;
      }
      putch(c);
    }
    putch(c);
    line[i] = '\0';

    int pos=0;
    while(line[pos] != ' ' && line[pos] != '\0') pos++;

    memcpy(cmd, line, pos);
    memcpy(arg, line + pos + 1, i - pos);
    cmd[pos] = '\0';
    arg[i - pos] = '\0';
    if (!strcmp(cmd, "ls")) {
      ls(arg);
    } else if(!strcmp(cmd, "cat")) {
      cat(arg);
    } else if(!strcmp(cmd, "objdump")) {
      objdump(arg);
    } else {
      printf("Unknown command: %s\n", cmd);
    }
  }

  return 0;
}
