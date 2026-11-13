/* PR tree-optimization/128672 */
/* { dg-do run } */

/* For X == (unsigned) INT_MIN, -X == X, so both conversions to int are
   INT_MIN and neither quotient may be computed as the negation of the
   other.  */

int g;

__attribute__ ((noipa)) int
f1 (unsigned x)
{
  g = (int) x / 2;
  return (int) -x / 2;
}

__attribute__ ((noipa)) int
f2 (unsigned x)
{
  g = (int) -x / 2;
  return (int) x / 2;
}

__attribute__ ((noipa)) int
f3 (int a, unsigned y)
{
  g = a / (int) y;
  return a / (int) -y;
}

__attribute__ ((noipa)) int
f4 (int a, unsigned y)
{
  g = a / (int) -y;
  return a / (int) y;
}

int
main ()
{
  unsigned x = (unsigned) __INT_MAX__ + 1;
  int m = -__INT_MAX__ - 1;
  if (f1 (x) != m / 2 || g != m / 2)
    __builtin_abort ();
  if (f2 (x) != m / 2 || g != m / 2)
    __builtin_abort ();
  if (f3 (m, x) != 1 || g != 1)
    __builtin_abort ();
  if (f4 (m, x) != 1 || g != 1)
    __builtin_abort ();
  return 0;
}
