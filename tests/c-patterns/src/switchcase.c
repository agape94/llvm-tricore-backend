int main()
{
  volatile int a = 5;
  int b = 1;

  switch(a)
  {
    case 1: b = 11; break;
    case 2: b = 10; break;
    case 3:
      if (b == 0)
        b = 0;
      else
        b = 3;
      break;
    case 4: b = 12; break;
    case 5: b = 14; break;
    default: b = 0;
  }

  return b;
}