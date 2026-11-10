struct ops
{
  long pad;
  int (*f) (int);
  int (*g) (int);
};

void __attribute__ ((noinline, noclone))
set_f (struct ops *o, int (*p) (int))
{
  o->f = p;
}
