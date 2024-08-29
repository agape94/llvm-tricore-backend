void somefunc()
{
  return;
}

int main()
{
  volatile int a;
  volatile int b;

  switch(a)
  {
    case 1: b = 12; break;

    case 2: b = 26; break;

    case 3:
      if (b == 0)
        b = 76;
      else
        b = 3;
      break;

    case 4:
      
      switch(b)
      {
        case 1:
          a = 5;
          a = 10;
          a = 15;
          a = a + a;
          break;
        case 2: a = 535; break;
        case 3: a = 354; break;
        case 4: a = 211; break;
        case 5: a = 632; break;
        case 6: a = 35654; break;
        case 7: a = 232; somefunc(); break;
        default: a = 5;
      }

      break;

    case 5: b = 52; break;

    default: b = 0;
  }

  return b;
}