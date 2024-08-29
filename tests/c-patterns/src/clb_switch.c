int func(int (*callback)(void)) {
  volatile int a = 10;
  a = callback();
  a++;
  return a;
}

int clb() {
  volatile int a;
  volatile int b;

  switch(a) {
    case 0: b = 1804289383; break;
    case 1: b = 846930886; break;
    case 2: b = 1681692777; break;
    case 3: b = 1714636915; break;
    case 4: b = 1957747793; break;
    default: b = -1;
  }
  return b;
}

int main() {
  volatile int a = func(clb);
  return a;
}
