#include <fs.h>
#include <system.h>
#include <ls.h>

unsigned int ls(const char *path) {
  dir_t *d = opendir(path);
  if (d) {
    struct dirent *dir;
    int index = 0;
    while(dir = readdir(d)) {
      printf("%s ", dir->name);
      index++;
    }
    printf("\n");
  } else {
    printf("Directory not found: %s\n", path);
  }
}
