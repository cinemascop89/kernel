#include <stdio.h>
#include <ext2.h>

struct mem_tag {
  void* next;
  int size;
};

long pow(long n, int exponent) {
  long ret = n;
  if (exponent == 0) return 1;
  while(--exponent > 0) ret *= n;
  return ret;
}

void putint(int val) {
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  int b = 10, tot_digits = 0, orig_val = val, curr_digit;

  if (val < 0) {
    val = -val;
  }
  do {
    tot_digits++;
  } while ((val /= b) > 0);
  curr_digit = tot_digits;
  do {
    printf("%d, %d\n",curr_digit, orig_val/pow(b, curr_digit - 1) - orig_val/pow(b, curr_digit)*b);
  } while (--curr_digit);

}

int main() {

  FILE *f;
  f = fopen("disk.img.bak", "rb");
  fseek(f, 1024, SEEK_SET);
  char *superblock = (char*)malloc(1024);
  fread(superblock, 1, 1024, f);
  ext2_fs_info_t *i = ext2_parse_superblock(superblock);
  printf("sys info:\ninodes:%d\nblocks:%d\nunallocated inodes:%d\nunallocated blocks:%d\n", i->total_inodes, i->total_blocks, i->unallocated_inodes, i->unallocated_blocks);
  printf("blocks per group:%d\ninodes per group:%d\n", i->blocks_per_group, i->inodes_per_group);
  printf("signature: 0x%x\nuid:%d\ngid:%d\n", i->signature, i->superuser_id, i->superuser_groupid);
  fclose(f);
  free(superblock);
  return 0;
}
