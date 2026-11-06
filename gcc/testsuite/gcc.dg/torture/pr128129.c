/* PR tree-optimization/128129 */
/* { dg-do run } */

/* ifcombine must not invert a floating point comparison without accounting
   for NaNs.  Every argument below compares unordered.  */

int sink;

__attribute__((noipa)) int
and_ne (double a, double b, int k)
{
  _Bool v;
  if (k)
    {
      v = a < b;
      sink = 1;
    }
  else
    {
      v = a < b;
      sink = 2;
    }
  if (!v)
    if (a != b)
      return 1;
  return 0;
}

__attribute__((noipa)) int
or_eq (double a, double b, int k)
{
  _Bool v;
  if (k)
    {
      v = a <= b;
      sink = 1;
    }
  else
    {
      v = a <= b;
      sink = 2;
    }
  if (!v)
    return 1;
  if (a == b)
    return 1;
  return 0;
}

__attribute__((noipa)) int
and_le (double a, double b, int k)
{
  _Bool v;
  if (k)
    {
      v = a < b;
      sink = 1;
    }
  else
    {
      v = a < b;
      sink = 2;
    }
  if (!v)
    if (a <= b)
      return 1;
  return 0;
}

int
main (void)
{
  double nan = __builtin_nan ("");

  if (and_ne (nan, 1.0, 1) != 1 || and_ne (1.0, nan, 0) != 1)
    __builtin_abort ();
  if (or_eq (nan, 1.0, 1) != 1 || or_eq (1.0, nan, 0) != 1)
    __builtin_abort ();
  if (and_le (nan, 1.0, 1) != 0 || and_le (1.0, nan, 0) != 0)
    __builtin_abort ();
  return 0;
}
