struct ops
{
  long pad;
  int (*f) (int);
  int (*g) (int);
};

int __attribute__ ((noinline)) fa (int x) { return x + 1; }
int __attribute__ ((noinline)) fb (int x) { return x + 100; }

struct ops gops = { 0, fa, fa };
