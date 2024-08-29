int main() {
  volatile int a;
  volatile int b;

  for (int i = 0; i < a; i++) {
    if (i == b) {
        b = 0;
        break;
    }
  }
  return b;
}