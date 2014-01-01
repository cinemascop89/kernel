#include <fs.h>
#include <ext2.h>
#include <lba.h>
#include <system.h>

partition_info_t* parse_partition_info(char* sector) {

  if (!sector[4]) { // If system id is 0, entry is empty
    return NULL;
  }
  int i;
  printf("parsed block:\n");
  for(i=0;i<16;i++)printf("%x ", sector[i]);
  partition_info_t *info =
    (partition_info_t*) malloc(sizeof(partition_info_t));

  info->active = sector[0];
  info->start_head = sector[1];
  info->start_sector = sector[2] & 0x3f;
  info->start_cylinder = (sector[2] & 0x3) + sector[3];
  info->system_id = sector[4];

  info->start_lba = sector[8] + sector[9]*0x100 + sector[10]*0x10000 + sector[11]*0x1000000;
  info->total_sectors = sector[12] + sector[13]*0x100 + sector[14]*0x10000 + sector[15]*0x1000000;
}

partition_info_t** init_partition_table(lba_drive_t *d) {
  char *sector = (char*)malloc(512);
  /* char sector[512]; */
  partition_info_t *p, *partitions[4];
  int i;
  for(i=0;i<512;i++) sector[i] = 0;
  lba_read_sectors(d, 0, 1, sector);
  for(i=0;i<16;i++)printf("%x ", sector[446+i]);
  printf("sectors read at %x\n", (unsigned int)sector);

  for(i=0; i<4; i++) {
    p = parse_partition_info((char*)(sector+446+i*16));
    partitions[i] = NULL;
    if (p) {
      p->drive = d;
      partitions[i] = p;
      printf("total sectors: %d\n", p->start_lba);

      switch (p->system_id) {
      case FS_TYPE_EXT2:
        ext2_init_fs(p);
        break;
      default:
        printf("unknown file system id:%x\n", p->system_id);
        break;
      }
    }
  }
}

void fs_read_sectors
(partition_info_t *partition, unsigned long address, unsigned short count, unsigned char *buff) {
  lba_read_sectors(partition->drive,
                   partition->start_lba + address,
                   count, buff);
}

void init_fs() {
  init_drives();

  init_partition_table(get_drive(LBA_PRIMARY_MASTER));
  printf("init bus as %x\n", get_drive(LBA_PRIMARY_MASTER));
}
