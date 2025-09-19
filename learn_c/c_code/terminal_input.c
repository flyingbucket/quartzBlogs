#include "stdio.h"
#include "string.h"

int main(void) {
  char name[25];
  printf("Using scanf to get input\n");
  // scanf writes input into a varible
  // scanf takes the address of the varible as a parammter
  // but in this case , name is the first address of the char arr, so we shoule
  // pass name to scanf instead of &name
  scanf("%24s", name);
  printf("Hello %s\n", name);

  // chear out buffer (delet the \n after scanf)
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
  printf("Using fget to get input\n");
  fgets(name, 25, stdin);
  name[strnlen(name, 25) - 1] = '\0';
  printf("Hello %s\n", name);
  return 0;
}
