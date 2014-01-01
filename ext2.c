#include <ext2.h>

ext2_superblock_t* ext2_parse_superblock(char *buff) {
  ext2_superblock_t *info =
    (ext2_superblock_t*)malloc(sizeof(ext2_superblock_t));
  info = (ext2_superblock_t*)buff;
  return info;
}

ext2_fs_info_t* ext2_init_fs(partition_info_t* p) {
  char buff[1024];
  fs_read_sectors(p, 2, 2, buff);
  ext2_superblock_t *sb = ext2_parse_superblock(buff);
  printf("signature: %x\n", sb->signature);
  printf("blocks:%d inodes:%d\n", sb->total_blocks, sb->total_inodes);
  printf("root uid:gid=%d:%d\n", sb->superuser_id, sb->superuser_groupid);

}
