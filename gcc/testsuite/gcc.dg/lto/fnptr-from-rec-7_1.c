struct ops
{
  long pad;
  int (*f) (int);
  int (*g) (int);
};

int __attribute__ ((noinline)) fb (int x) { return x + 100; }

void __attribute__ ((noinline, noclone))
set_fb (struct ops *o)
{
  o->f = fb;
}
