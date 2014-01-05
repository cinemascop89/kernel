#include <stdio.h>
#include <str.h>

int main() {
  char in[] = "/", *token;

  token = strtok(in, "/");
  if (token==NULL) {
    printf("string empty\n");
    return 0;
  }
  printf("part: %s\n", token);
  for(;;) {
    token = strtok(NULL, "/");
    if (token == NULL)
      break;
    printf("part: %s\n", token);
  }
}
