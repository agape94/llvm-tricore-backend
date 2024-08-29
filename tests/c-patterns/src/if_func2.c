int func1(int arg) {
  volatile int a = arg + 2;
  if (a < 10)
    a += 10;
  else
    a -= 10;
  return a;
}

int func2(int arg) {
  volatile int a = arg - 2;
  if (a > 10)
    a += 20;
  else
    a -= 20;
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