// #include <stdio.h>

int func1(int arg) {
  int a = arg + 2;
  return a;
}

int func2(int arg) {;
  int a = arg + 4;
  return a;
}

int main(int argc, char* argv[]) {
  int(*funcs[])(int) = { func1, func2 };

  int i;

  return funcs[i](10);

  return 0;
  
}