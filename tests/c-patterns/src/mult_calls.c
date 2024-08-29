int func1() {
  return 20;
}

int func2() {
  return 40;
}

int main() {
  volatile int a = 10;
  volatile int b = 20;
  a += func1();
  b += func2();
  return a + b;
}