#include <system.h>
#include <fs.h>
#include <cat.h>

unsigned int cat(char *path) {
  file_t *f = fopen(path, 'r');
  if (!f) {
    printf("File not found: %s\n", path);
    return 1;
  }
  fseek(f, 0, SEEK_END);
  int file_len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *contents = (char*)malloc(file_len+1);
  fread(contents, 1, file_len, f);
  contents[file_len] = '\0';
  printf("%s\n", contents);
  fclose(f);
  free(contents, file_len);

  return 0;
}
