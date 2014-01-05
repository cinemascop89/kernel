#include <system.h>
#include <types.h>
#include <str.h>
#include <colors.h>
#include <lba.h>
#include <ext2.h>
#include <fs.h>
#include <multiboot.h>

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count) {
  int i;
  for (i = 0; i < count; i++) {
    dest[i] = src[i];
  }
  return dest;
}

unsigned char *memset(unsigned char *dest, unsigned char val, int count) {
  int i;
  for (i = 0; i < count; i++) {
    dest[i] = val;
  }
  return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count) {
  unsigned short i;
  for (i = 0; i < count; i++) {
    dest[i] = val;
  }
  return dest;
}

void welcome_message() {
  unsigned char *welcome = "Welcome!\n";
  int i;
  for (i = 0; i < strlen(welcome); i++) {
    settextcolor((i % 14) + 1, BLACK);
    putch(welcome[i]);
  }
  settextcolor(WHITE, BLACK);

}

void test_allocation() {
  int *a, *b;
  a = (int*) malloc(sizeof(int));
  b = (int*) malloc(sizeof(int));
  *a = 123456;
  *b = 678905;

  printf("a=%d, b=%d\n", *a, *b);
}

void test_lba() {
  unsigned char buff[1024];
  lba_drive_t* drive = get_drive(LBA_PRIMARY_MASTER);
  lba_read_sectors(drive, 0, 1, buff);
  partition_info_t *i = parse_partition_info((char*)(buff+446));

  lba_read_sectors(drive, i->start_lba+2, 1, buff);
  ext2_superblock_t *ei = ext2_parse_superblock(buff);
  printf("signature: %x\n", ei->signature);
}

void cpuid(){
   unsigned int eax, ebx, ecx, edx;
   /* Invoke CPUID level 0 and put the results in the eax/ebx/ecx/edx variables */
   __asm__ volatile("cpuid" : "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0x00));
   char vendor_id[13];
   vendor_id[12] = NULL;
   vendor_id[11] = ecx >> 24;
   vendor_id[10] = (ecx >> 16) & 0xff;
   vendor_id[9] = (ecx >> 8) & 0xff;
   vendor_id[8] = ecx & 0xff;
   vendor_id[7] = edx >> 24;
   vendor_id[6] = (edx >> 16) & 0xff;
   vendor_id[5] = (edx >> 8) & 0xff;
   vendor_id[4] = edx & 0xff;
   vendor_id[3] = ebx >> 24;
   vendor_id[2] = (ebx >> 16) & 0xff;
   vendor_id[1] = (ebx >> 8) & 0xff;
   vendor_id[0] = ebx & 0xff;

   printf("CPU: %s\n", vendor_id);
}
unsigned char inportb(unsigned short _port) {
  unsigned char rv;
  __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
  return rv;
}

unsigned int inportw(unsigned short _port) {
  unsigned int rv;
  __asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (_port));
  return rv;
}

/* void inportsw(unsigned short _port, void* buff, unsigned long count) { */
/*   unsigned int *rv; */
/*   __asm__ volatile ( */
/*     "cld; rep; insw " */
/*     : "=D" (buff), "=c"(count) */
/*     : "d"(_port), "0"(buff), "1"(count)); */
/*   return rv; */
/* } */

void inportsw(unsigned short port, void *addr, unsigned long count)
{
  __asm__ __volatile__ (
    "cld ; rep ; insw "
    : "=D" (addr), "=c" (count)
    : "d"(port), "0"(addr), "1" (count)
    : "memory"
  );
}

void outportb(unsigned short _port, unsigned char _data) {
  __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void outportw(unsigned short _port, unsigned int _data) {
  __asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

/* void outportsw(unsigned short _port, unsigned short _data) { */
/*   __asm__ __volatile__ ("outsw %1, %0" : : "dN" (_port), "=S" (_data), "0"(_data) : "%si"); */
/* } */


main(multiboot_info_t *mbd, unsigned int magic) {

  init_paging();
  gdt_install();
  idt_install();
  isrs_install();
  irq_install();

  __asm__ __volatile__ ("sti");

  timer_install();
  keyboard_install();

  init_video();
  welcome_message();
  cpuid();
  printf("GRUB flags: %b\n", mbd->flags);

  //  if (mbd->flags & MULTIBOOT_MEM_FLAG) {
  if (0) {
    unsigned long total_mem = 0, entry_size;
    printf("Memory map:\n");
    memory_map_t *mmap = mbd->mmap_addr;
    while (mmap < mbd->mmap_addr + mbd->mmap_length) {
      if (mmap->type == 0x01) {
        entry_size = (mmap->length_low + mmap->length_high * 0xffffffff);
        total_mem += entry_size;
        printf("0x%x (%d kbytes)\n",
               mmap->base_addr_low + mmap->base_addr_high * 0xffffffff,
               entry_size / 1024);
      }
      mmap = (memory_map_t*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
    }
    printf("Available memory: %d mbytes\n", total_mem / 1024 / 1024);
  }
  init_mem((memory_map_t*)mbd->mmap_addr,
           mbd->mmap_addr + mbd->mmap_length);

  /* char in[] = "/boot", *tok; */
  /* strtok((char*)"/boot", "/"); */
  /* while(tok = strtok(NULL, "/")) printf("%s\n", tok); */
  /* printf("final string: %s\n"); */

  init_fs();
  //  test_lba();

  for (;;);
}
