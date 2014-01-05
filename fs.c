#include <fs.h>
#include <ext2.h>
#include <lba.h>
#include <system.h>
#include <types.h>

fs_node_t *fs_root = NULL;

partition_info_t* parse_partition_info(char* sector) {

  if (!sector[4]) { // If system id is 0, entry is empty
    return NULL;
  }
  int i;
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
  partition_info_t *p, *partitions[4];
  int i;

  lba_read_sectors(d, 0, 1, sector);

  for(i=0; i<4; i++) {
    p = parse_partition_info((char*)(sector+446+i*16));
    partitions[i] = NULL;
    if (p) {
      p->drive = d;
      partitions[i] = p;
      printf("partition %d total sectors: %d\n", i, p->start_lba);

      switch (p->system_id) {
      case FS_TYPE_EXT2:
        fs_root = ext2_root_node(ext2_init_fs(p));

        int i=0;
        struct dirent* entry;
        char dir[] = "/boot/grub/..";
        fs_node_t *n = fs_finddir(fs_root, dir);
        if (n) {
          while(entry = fs_readdir(n, i++)) {
            printf("%s - %d\n", entry->name, entry->inode);
          };
          printf("all done.\n");
        } else {
          printf("dir not found\n");
        }

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

unsigned int fs_read
(fs_node_t *node, unsigned int offset, unsigned int size, char *buff) {
  if (node->read != 0)
    return node->read(node, offset, size, buff);
  else
    return 0;
}

unsigned int fs_write
(fs_node_t *node, unsigned int offset, unsigned int size, char *buff) {
  if (node->write != 0)
    return node->write(node, offset, size, buff);
  else
    return 0;
}

void fs_open
(fs_node_t *node, unsigned char read, unsigned char write) {
  if (node->open != 0)
    node->open(node);
}

void fs_close
(fs_node_t *node) {
  if (node->close != 0)
    node->close(node);
}

struct dirent *fs_readdir
(fs_node_t *node, unsigned int index) {
  if ((node->flags&0x7) == FS_DIRECTORY && node->readdir != 0 )
    return node->readdir(node, index);
  else
    return 0;
}

fs_node_t *fs_finddir
(fs_node_t *node, char *name) {
  if ((node->flags&0x7) == FS_DIRECTORY && node->finddir != 0 )
    return node->finddir(node, name);
  else
    return 0;
}

void init_fs() {
  init_drives();
  init_partition_table(get_drive(LBA_PRIMARY_MASTER));
}
