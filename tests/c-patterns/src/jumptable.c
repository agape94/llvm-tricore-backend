int main(int argc, char *argv[]) {
  int x = argc;
  switch (argv[0][0]) {
    case 'a':
      x = x + 5;
    case 'b':
      x = x / 2;
      break;
    case 'z':
      x = x * 5;
      break;
    case '5':
      x = 2;
      break;
    case 'B':
      x = 487 - 2*x;
      break;
    case 'l':
      x = 22 + 213 / x - x * x;
      break;
    default:
      x = -1;
  }
  return 622 * x - 3;
}
