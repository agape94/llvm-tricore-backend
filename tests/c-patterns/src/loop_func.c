int func1(int arg) {
  int a = arg + 2;
  return a;
}

int main() {
  
  int retval = 0;

  for(int i = 0; i < 10; i++) {
    if (i == 4)
      retval = func1(10);
  }

  return retval;
}