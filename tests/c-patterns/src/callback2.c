void clb1() {
  volatile int a = 10;
  a++;
}

void clb2() {
  volatile int a = 20;
  a++;
}

int func(void (*clb)(void)) {
  volatile int b = 20;
  b = 10 * b;
  clb();
}

int main() {
  volatile int c = 30;
  c++;
  func(clb1);
  c++;
  func(clb2);
  return 0;
}