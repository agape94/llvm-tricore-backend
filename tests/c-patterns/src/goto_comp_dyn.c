int main(int argc, char *argv[]) {
  int c = 0;

  void *labels[] = { &&L1, &&L2, &&L3 };

  int volatile i;

  if(i >= 0 && i <= 2) {
    goto *labels[i];
  }
  goto L4;

  L1:
  c = 1;
  goto L4;
  
  L2:
  c = 2;
  goto L4;

  L3:
  c = 3;
  goto L4;

  L4:
  return c;
}