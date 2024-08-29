void f5() {
  
}

void f4() {
  f5();
}

void f3() {
  f4();
}

void f2() {
  f3();
}

void f1() {
  f2();
}

int main() {
  f1();
  return 0;
}