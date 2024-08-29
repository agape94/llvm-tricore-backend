int main(int argc, char* argv[]) {
  
  int volatile choice;

  if (choice >= 1) {
     choice = 10;
  } else {
     choice = 20;
  }

  return choice;
}