__attribute__ ((section (".foo"), noinline)) inline int
f (int x)
{
  __asm__ ("");
  return x + 1;
}

__attribute__ ((section (".foo"), noinline)) inline int
g (int x)
{
  __asm__ ("");
  return x + 2;
}

int
caller (int x)
{
  return f (x) + g (x);
}
