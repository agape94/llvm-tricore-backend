void func1()
{
  int a = 5;
  a = a + a;
}


void func2()
{
  int b = 6;
  b = b + b;
}

void(*ptr[])(void) = { func1, func2 };

int main(int argc, char* argv[])
{
  ptr[0]();
  ptr[1]();
  return 0;
}