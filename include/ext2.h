#ifndef __EXT2_H
#define __EXT2_H

#include <fs.h>

#define EXT2_FS_STATE_CLEAN 1
#define EXT2_FS_STATE_ERROR 2

#define EXT2_ERROR_BEHAVIOUR_IGNORE  1
#define EXT2_ERROR_BEHAVIOUR_REMOUNT 2
#define EXT2_ERROR_BEHAVIOUR_PANIC   3

typedef struct ext2_superblock {
    unsigned int total_inodes;
  unsigned int total_blocks;
  unsigned int superuser_blocks;
  unsigned int unallocated_blocks;
  unsigned int unallocated_inodes;

  unsigned int superblock_block_num;
  unsigned int block_size_log;
  unsigned int fragment_size_log;

  unsigned int blocks_per_group;
  unsigned int fragments_per_group;
  unsigned int inodes_per_group;

  unsigned int last_mount; //UNIX timestamps
  unsigned int last_write;

  unsigned short mounts_since_last_fsck;
  unsigned short allowed_mounts_before_fsck;

  unsigned short signature;
  unsigned short state;
  unsigned short err_behaviour;

  unsigned short minor_version;
  unsigned int last_fsck;
  unsigned int interval_fsck;
  unsigned int fs_creator_id;
  unsigned int major_version;

  unsigned short superuser_id;
  unsigned short superuser_groupid;

  // TODO: add extended fields

} ext2_superblock_t;

typedef struct ext2_fs_info {
  ext2_superblock_t *superblock;
  partition_info_t *partition;
} ext2_fs_info_t;

/* typedef struct ext2_bgd { */
/*   unsigned int bock_usage_bmap; */
/*   unsigned int inode_usage_bmap; */
/*   unsigned int inode_table; */
/*   unsigned short unallocated_blocks; */
/*   unsigned short unallocated_inodes; */
/*   unsigned short directories; */
/* } ext2_bgd_t; */

ext2_fs_info_t* ext2_init_fs(partition_info_t* p);
#endif
