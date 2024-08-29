int main() {
  volatile int a;

  if (a > 0) {
    a += 10;
    return 1;
  }

  if (a < 0) {
    a += 20;
  }

  if (a > 100) {
    a += 100;
    return 2;
  }

  a += 3;

  return 0;
}