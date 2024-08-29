void clb() {
  volatile a = 10;
  a++;
}

int func(void (*clb)(void)) {
  volatile b = 20;
  b = 10 * b;
  clb();
}

int main() {
  volatile c = 30;
  c++;
  c++;
  func(clb);
  return 0;
}