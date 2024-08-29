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

  return func1p(10);

  return 0;
}