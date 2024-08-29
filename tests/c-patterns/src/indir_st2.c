int func1(int arg) {
  int a = arg + 2;
  return a;
}

int func2(int arg) {
  int a = arg + 4;
  return a;
}

int main() {
  
  int(*func1p)(int) = func1;
  int(*func2p)(int) = func2;

  volatile int choose;
  int(*funcp)(int);

  if (choose == 0)
    funcp = func1;
  else
    funcp = func2;

  return funcp(10);

  return 0;
}