int func1(int arg) {
  volatile int a = arg + 2;
  return a;
}

int func2(int arg) {
  volatile int a = arg + 2;
  return a;
}

int main(int argc, char* argv[]) {
  
  int volatile choice;

  if (choice >= 1) {
     return func1(10);
  } else {
     return func2(20);
  }

  return 0;
}