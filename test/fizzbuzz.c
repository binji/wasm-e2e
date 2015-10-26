void puts(const char*);
void print(int);

int main() {
  int i;
  for (i = 1; i < 20; ++i) {
    if (i % 15 == 0)
      puts("fizzbuzz");
    else if (i % 5 == 0)
      puts("buzz");
    else if (i % 3 == 0)
      puts("fizz");
    else
      print(i);
  }
  return 0;
}
