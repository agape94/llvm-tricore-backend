int main(int argc, char* argv[]) {
  int a = 0;
  volatile int num;
  int i = 0;

  while(i < num) {
    a += 2;
    i++;
  }
  return a;
}