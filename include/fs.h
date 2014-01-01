#ifndef __FS_H
#define __FS_H

#include <lba.h>

#define FS_TYPE_EXT2  0x83
#define FS_TYPE_FAT32 0x0b

typedef struct partition_info {
  char active;
  char start_head;
  char start_sector;
  unsigned short start_cylinder;
  char system_id;
  char end_head;
  char end_sector;
  unsigned short end_cylinder;
  unsigned int start_lba;
  unsigned int total_sectors;

  lba_drive_t *drive;
} partition_info_t;

partition_info_t* parse_partition_info(char* sector);
partition_info_t** init_partition_table(lba_drive_t *d);
void fs_read_sectors(partition_info_t *partition,
                     unsigned long address,
                     unsigned short count,
                     unsigned char *buff);
#endif
