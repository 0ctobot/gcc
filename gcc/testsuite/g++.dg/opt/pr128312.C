// PR tree-optimization/128312
// { dg-do run }
// { dg-options "-O2" }

struct S { int a[8]; };

__attribute__((noipa)) S
f (int t)
{
  S r;
  for (int i = 0; i < 8; i++)
    r.a[i] = 42;
  if (t)
    throw 1;
  return r;
}

__attribute__((noipa)) int
g (int t)
{
  S d;
  for (int i = 0; i < 8; i++)
    d.a[i] = 0;
  try
    {
      d = f (t);
    }
  catch (int)
    {
      return d.a[0];
    }
  return -1;
}

int
main ()
{
  if (g (1) != 0)
    __builtin_abort ();
}
