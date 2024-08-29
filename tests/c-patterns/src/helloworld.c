// #include <stdio.h>

int main()
{
  int volatile a = 10;
  if (a > 10)
    // printf("Hello world!\n");
    return 100;
  else
    // printf("Hallo Welt!\n");
    return 200;
  return 0;
}