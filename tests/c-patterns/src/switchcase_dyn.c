int main(int argc, char* argv[]) {
  volatile int a;
  int s;
  

  switch(s) {
    case 1: a = 111; break;
    case 2: a = 12; break;
    case 3: a = 23; break;
    case 4:
      if (a == 0)
        a = 0;
      else
        a = 3;
      break;;
    case 5: a = 54; break;
    case 6: a = 22; break;
    case 9: a = 66; break;
    default: a = 24;
  }

  return a;
}