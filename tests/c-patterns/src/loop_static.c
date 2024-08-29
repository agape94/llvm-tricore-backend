int main() {

  volatile int a = 0;
  
  for (int i = 0; i < 20; i++) {
    a += 2;
  }

  return a;
}