void func1(int arg) {
  int a = arg + 2;
}

void func2(int arg) {;
  int a = arg + 4;
}

int main(int argc, char* argv[]) {
  void(*funcs[])(int) = { func1, func2 };

  int i;

  if(i > 1 && i < 0)
    return 0;

  funcs[i](10);

  return 0;
  
}