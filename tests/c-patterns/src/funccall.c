int func(int a) {
  a++;
  return a;
}

int main() {
  int volatile a = 10;
  a = func(a);
  return 0;
}