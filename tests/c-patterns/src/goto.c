int main() {
  
  int volatile a = 0;

  L2:
  if ( a >= 10 ) {
    goto L1;
  }

  a++;
  goto L2;

  L1:
  return a;
}