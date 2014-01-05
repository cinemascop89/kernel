#include <system.h>
#include <types.h>
#include <fs.h>
#include <ext2.h>

struct dirent* readdir(struct fs_node* node, unsigned int index);
fs_node_t* finddir(fs_node_t *node, char *name);

ext2_superblock_t* ext2_parse_superblock(char *buff) {
  ext2_superblock_t *info =
    (ext2_superblock_t*)malloc(sizeof(ext2_superblock_t));
  memcpy(info, buff, sizeof(ext2_superblock_t));
  if (info->major_version < 1) {
    info->first_free_inode = 11;
    info->inode_size = 128;
  }
  return info;
}

ext2_bgd_entry_t* ext2_parse_bgd(char *buff) {
  ext2_bgd_entry_t *bgde =
    (ext2_bgd_entry_t*)malloc(sizeof(ext2_bgd_entry_t));
  memcpy(bgde, buff, sizeof(ext2_bgd_entry_t));
  printf("block usage: %d, inode usage: %d, directories:%d\n",
         bgde->block_usage_bmap,
         bgde->inode_usage_bmap,
         (int)bgde->directories);
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
  //  printf("reading inode %d\n", inode_num);
  ext2_superblock_t *sb = fsinfo->superblock;
  int inode_blockgroup = (inode_num - 1)/sb->inodes_per_group;
  int it_addr = fsinfo->bgdt[inode_blockgroup]->inode_table;
  int block_size = ext2_get_block_size(sb);
  char *block = (char*)malloc(block_size);
  //  ext2_read_blocks(fsinfo, it_addr, 1, block);

  int inode_index = (inode_num - 1) % sb->inodes_per_group;
  int inode_block = (inode_index * sb->inode_size) / block_size;
  /* printf("inode size:%d, block size:%d, block group:%d\n", sb->inode_size, block_size, inode_blockgroup); */
  /*   printf("inode: %d, table addr: %d, inode_block: %d, index:%d, at pos %d\n", inode_num, it_addr, inode_block, inode_index, inode_index*sb->inode_size - inode_block*block_size); */
  ext2_inode_t *inode = (ext2_inode_t*)malloc(sizeof(ext2_inode_t));
  ext2_read_blocks(fsinfo, inode_block + it_addr, 1, block);
  memcpy(inode, block + inode_index*sb->inode_size - inode_block*block_size, sizeof(ext2_inode_t));
  //  printf("inode type:%x, uid:%d\n", inode->type_and_perm, inode->uid);

  return inode;
}

ext2_dir_entry_t* ext2_read_dir_entry
(ext2_fs_info_t *fsinfo, ext2_inode_t *inode) {
  int block_size = ext2_get_block_size(fsinfo->superblock);
  char *block = (char*)malloc(block_size);
  ext2_read_blocks(fsinfo, inode->direct_pointers[0], 1, block);

  unsigned char name_len = block[6];
  ext2_dir_entry_t *entry =
    (ext2_dir_entry_t*)malloc(sizeof(ext2_dir_entry_t));
  memcpy(entry, block, sizeof(ext2_dir_entry_t));
  //  entry = (ext2_dir_entry_t*)block;
  entry->name = (char*)malloc(name_len+1);
  memcpy(entry->name, (char*)(block+8), name_len);
  entry->name[name_len] = '\0';
}

ext2_fs_info_t* ext2_init_fs(partition_info_t* p) {
  ext2_fs_info_t* info =
    (ext2_fs_info_t*)malloc(sizeof(ext2_fs_info_t));
  info->partition = p;
  char buff[1024];
  fs_read_sectors(p, 2, 2, buff);
  ext2_superblock_t *sb = ext2_parse_superblock(buff);
  info->superblock = sb;

  char *block = (char*)malloc(ext2_get_block_size(sb));
  if (ext2_get_block_size(sb) == 1024)
    ext2_read_blocks(info, 2, 1, block);
  else
    ext2_read_blocks(info, 1, 1, block);

  int i, bgdt_size = sb->total_blocks / sb->blocks_per_group;
  if (bgdt_size*sb->blocks_per_group < sb->total_blocks) bgdt_size++; // Rounding up
  ext2_bgd_entry_t **bgdt =
    (ext2_bgd_entry_t**)malloc(sizeof(ext2_bgd_entry_t*)*bgdt_size);
  for(i=0;i < bgdt_size; i++)
    bgdt[i] = ext2_parse_bgd(block + i*32);
  info->bgdt = bgdt;

  return info;
}

fs_node_t* ext2_inode_to_fs
(ext2_fs_info_t *info, unsigned int inode_n, ext2_inode_t* inode) {
  fs_node_t* node = (fs_node_t*)malloc(sizeof(fs_node_t));

  node->name[0] = 'a';
  node->uid = inode->uid;
  node->gid = inode->gid;
  node->flags = FS_DIRECTORY; //FIX this!!
  node->inode = inode_n;
  node->length = inode->size_lower; // Also use the hi bytes
  if (info->superblock->major_version >= 1)
    node->length += inode->size_higher*0x100000000;

  node->fs = info;

  node->finddir = finddir;
  node->readdir = readdir;

  return node;

}

fs_node_t* ext2_root_node(ext2_fs_info_t *info) {
  ext2_inode_t *root_inode = ext2_read_inode(info, EXT2_ROOT_INODE);
  fs_node_t* root = ext2_inode_to_fs(info,
                                     EXT2_ROOT_INODE,
                                     root_inode);
  root->fs = info;
  return root;
}

struct dirent* readdir(struct fs_node* node, unsigned int index) {
  ext2_fs_info_t *info = (ext2_fs_info_t*)node->fs;

  char *block = (char*)malloc(ext2_get_block_size(info->superblock));
  ext2_inode_t *inode = ext2_read_inode(info, node->inode);
  int curr_entry = 0, entry_offset = 0, entry_size;
  ext2_read_blocks(info, inode->direct_pointers[0], 1, block);
  while(curr_entry != index) {
    entry_size = block[entry_offset + 4] + block[entry_offset + 5]*0x10;
    if (!entry_size)
      return NULL;
    entry_offset += entry_size;
    curr_entry++;
  }
  unsigned int e_inode;
  int i;
  //  for(i=0;i<4;i++) printf("%x ", block[entry_offset+i]);
  e_inode = block[entry_offset] + block[entry_offset+1]*0x100 + block[entry_offset+2]*0x10000 + block[entry_offset+3]*0x1000000;
  if (!e_inode)
    return NULL;

  struct dirent* entry = (struct dirent*)malloc(sizeof(struct dirent));
  entry->inode = e_inode;

  unsigned char name_len = block[entry_offset + 6];
  memcpy(entry->name, (char*)(block+entry_offset+8), name_len);
  entry->name[name_len] = '\0';

  return entry;
}

fs_node_t* finddir(fs_node_t *node, char *name) {
  ext2_fs_info_t *info = (ext2_fs_info_t*)node->fs;
  fs_node_t* dir_node = node;
  struct dirent *curr_dirent;// = node->readdir(dir_node, 0);
  int curr_index = 0;

  if (name[0] == '/') name++;
  int i=0;
  while(name[i] != '/' && name[i] != '\0') i++;
  if (i == 0) {
    return node;} // name == "/" or name == ""
  char *dir = (char*)malloc(i);//strpbrk(name, "/");
  memcpy(dir, name, i);
  if (dir[i] == '/') dir[i] = '\0';

  struct dirent *dcopy = (struct dirent*)malloc(sizeof(struct dirent));
  if (dir) {
    while(curr_dirent = node->readdir(node, curr_index)) {
      if (!strcmp(dir, curr_dirent->name)) {

        memcpy(dcopy, curr_dirent, sizeof(struct dirent));
        ext2_inode_t *ino = ext2_read_inode(
          info,
          curr_dirent->inode
        );
        fs_node_t *n = node->finddir(
          ext2_inode_to_fs(
            info,
            dcopy->inode,
            ino
          ),
          name + i);
        return n;
      }
      curr_index++;
    }
    return NULL; // not found
  } else {
    return node;
  }









  /* if (!dir) */
  /*   return dir_node; */

  /* while(dir) { */
  /*   curr_index = 0; */
  /*   printf("entering %s\n", dir); */
  /*   while(curr_dirent = dir_node->readdir(dir_node, curr_index)) { */
  /*     printf("checking : %s - %d\n", curr_dirent->name, curr_dirent->inode); */
  /*     if (!strcmp(dir, curr_dirent->name)) { */
  /*       dir_node = ext2_inode_to_fs(info, curr_dirent->inode, ext2_read_inode(info, curr_dirent->inode)); */
  /*       printf("occurrence found: %s, inode: %d\n", curr_dirent->name, curr_dirent->inode); */
  /*       break; */
  /*     } */
  /*     curr_index++; */
  /*   } */
  /*   if (curr_dirent == NULL) { */
  /*     printf("curr_dirent is null\n"); */
  /*     return NULL; // Directory not found */
  /*   } */
  /*   printf("next level...\n"); */

  /*   dir = strtok(NULL, "/"); */
  /* }; */
  /* printf("finished searching, inode:%d\n", dir_node->inode); */

  /* return dir_node; */

}
