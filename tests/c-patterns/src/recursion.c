int fac(unsigned int a) {
  if (a == 0) {
    return 1;
  } else {
    return a * fac(a - 1);
  }
}

int main() {
  volatile unsigned int a;
  unsigned int res = fac(a);
  return res;
}