int func1() {
  volatile int a = 10;
  a += 10;
  return a;
}

int func2() {
  volatile int a = 20;
  a += 30;
  return a;
}

int main() {
  volatile int a = 10;
  a += 10;
  func1();
  a += 10;
  func2();
  a += 10;
  func1();
  a += 10;
  func2();
  return 0;
}