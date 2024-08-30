int func1(int a) {
  volatile int b = a;
  return b + 10;
}

int func2(int a) {
  volatile int b = a;
  return b + 20;
}

int func3(int a) {
  volatile int b = a;
  return b + 30;
}

int func4(int a) {
  volatile int b = a;
  return b + 40;
}

int main() {
  int size = 4;
  int (*funcps[])(int) = {
    &func1,
    &func2,
    &func3,
    &func4
  };
  volatile int result = 0;

  for (int i = 0; i < size; i++) {
    result += funcps[i](result);
  }

  return 0;
}