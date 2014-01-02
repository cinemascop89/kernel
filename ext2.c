#include <system.h>
#include <ext2.h>

ext2_superblock_t* ext2_parse_superblock(char *buff) {
  ext2_superblock_t *info =
    (ext2_superblock_t*)malloc(sizeof(ext2_superblock_t));
  info = (ext2_superblock_t*)buff;
  if (info->major_version < 1) {
    info->first_free_inode = 11;
    info->inode_size = 128;
  }
  return info;
}

ext2_bgd_entry_t* ext2_parse_bgd(char *buff) {
  ext2_bgd_entry_t *bgde =
    (ext2_bgd_entry_t*)malloc(sizeof(ext2_bgd_entry_t));
  bgde = (ext2_bgd_entry_t*)buff;
  printf("block usage: %d, inode usage: %d, directories:%d\n",bgde->block_usage_bmap, bgde->inode_usage_bmap, (int)bgde->directories);
  return bgde;
}

unsigned int ext2_get_block_size(ext2_superblock_t *sb) {
  return (1024 << sb->block_size_log);
}

void ext2_read_blocks
(ext2_fs_info_t *fsinfo, unsigned int block, unsigned short count, char *buff) {
  unsigned int block_size = ext2_get_block_size(fsinfo->superblock);
  fs_read_sectors(fsinfo->partition,
                  block*block_size/512,
                  count*block_size/512,
                  buff);
}

ext2_inode_t* ext2_read_inode
(ext2_fs_info_t *fsinfo, unsigned int inode_num) {
  ext2_superblock_t *sb = fsinfo->superblock;
  int inode_blockgroup = (inode_num - 1)/sb->inodes_per_group;
  int it_addr = fsinfo->bgdt[inode_blockgroup]->inode_table;
  int block_size = ext2_get_block_size(sb);
  char block[block_size];
  printf("it_addr:%d\n", it_addr);
  ext2_read_blocks(fsinfo, it_addr, 1, block);

  int inode_index = (inode_num - 1) % sb->inodes_per_group;
  ext2_inode_t *inode = (ext2_inode_t*)malloc(sizeof(ext2_inode_t));
  inode = (ext2_inode_t*)(block + inode_index*sb->inode_size);

  int inode_block = (inode_index * sb->inode_size) / block_size;

  printf("inode block: %d, index:%d\n", inode_block, inode_index);
  //  ext2_read_blocks(fsinfo, inode_block, 1, block);

  return inode;
}

ext2_dir_entry_t* ext2_read_dir_entry
(ext2_fs_info_t *fsinfo, ext2_inode_t *inode) {
  int block_size = ext2_get_block_size(fsinfo->superblock);
  char block[block_size];
  printf("pointer 0 at %d\n", inode->direct_pointers[0]);
  ext2_read_blocks(fsinfo, inode->direct_pointers[0], 1, block);
  int i;
  for(i=0;i<16;i++) printf("%x ", block[i]);
  unsigned char name_len = block[6];
  ext2_dir_entry_t *entry =
    (ext2_dir_entry_t*)malloc(sizeof(ext2_dir_entry_t));
  entry = (ext2_dir_entry_t*)block;
  entry->name = (char*)malloc(name_len+1);
  memcpy(entry->name, (char*)(block+8), name_len);
  entry->name[name_len] = '\0';
  printf("entry name:%s\n", entry->name);
}

ext2_fs_info_t* ext2_init_fs(partition_info_t* p) {
  ext2_fs_info_t* info =
    (ext2_fs_info_t*)malloc(sizeof(ext2_fs_info_t));
  info->partition = p;
  char buff[1024];
  fs_read_sectors(p, 2, 2, buff);
  ext2_superblock_t *sb = ext2_parse_superblock(buff);
  info->superblock = sb;
  printf("volume %s last mount at %s\n", sb->vol_name, sb->mount_path);
  printf("blocks:%d inodes:%d\n", sb->total_blocks, sb->total_inodes);
  printf("root uid:gid=%d:%d\n", sb->superuser_id, sb->superuser_groupid);

  char *block = malloc(ext2_get_block_size(sb));
  if (ext2_get_block_size(sb) == 1024)
    ext2_read_blocks(info, 2, 1, block);
  else
    ext2_read_blocks(info, 1, 1, block);
  int i, bgdt_size = sb->total_blocks / sb->blocks_per_group;
  ext2_bgd_entry_t **bgdt =
    (ext2_bgd_entry_t**)malloc(sizeof(ext2_bgd_entry_t*)*bgdt_size);
  for(i=0;i < bgdt_size; i++)
    bgdt[i] = ext2_parse_bgd(block + i*32);
  info->bgdt = bgdt;

  ext2_inode_t *root_inode = ext2_read_inode(info, 2);
  ext2_read_dir_entry(info, root_inode);

  return info;
}
