void clb() {
  volatile int a = 10;
  a++;
}

int func(void (*clb)(void)) {
  volatile int b = 20;
  b = 10 * b;
  clb();
  return b;
}

int main() {
  volatile int c = 30;
  c++;
  c++;
  func(clb);
  return 0;
}