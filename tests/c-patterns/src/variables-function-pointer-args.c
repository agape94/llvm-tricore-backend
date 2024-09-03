int GLOBAL_VAL = 1;

int mean_value(int *a, int b) {
  return (*a + b) / 2;
}

int main()
{
    int a = 100;
    int b = a;
    int c = mean_value(&a, b);

    c = c + a + b;

    if (c >= 2) {
        GLOBAL_VAL += 3;
    }
    GLOBAL_VAL += 5;
    return c;
}