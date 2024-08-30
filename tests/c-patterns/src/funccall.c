int func(int a) {
  a++;
  return a;
}

int main() {
  int a = 10;
  a = func(a);
  return 0;
}