#include <stdio.h>
#include <string.h>

int main() {
  FILE *f = fopen("disk.img", "rb");
  int i = 0;

  unsigned char jmp[3];
  fread(jmp, 1, 3, f);
  printf("jump: %x %x %x\n", jmp[0], jmp[1], jmp[2]);

  unsigned char oem[9], vname[9];
  fread(oem, 1, 8, f);
  fread(vname, 1, 8, f);
  oem[8] = '\0';
  vname[8] = '\0';
  printf("OEM ident: %s, %s\n", oem, vname);

  unsigned short sector_size;
  fread(&sector_size, sizeof(sector_size), 1, f);
  printf("bytes per sector: %d\n", sector_size);

  fclose(f);
  return 0;
}
