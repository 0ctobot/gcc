/* PR tree-optimization/128672 */
/* With -fwrapv, -A is defined for A == INT_MIN and equals A, so A / B
   isn't the negation of -A / B even when the program computes -A.  */
/* { dg-do run } */
/* { dg-additional-options "-fwrapv" } */

int g;

__attribute__ ((noipa)) int
f1 (int a)
{
  g = -a / 2;
  return a / 2;
}

__attribute__ ((noipa)) int
f2 (int a)
{
  g = a / 2;
  return -a / 2;
}

__attribute__ ((noipa)) int
f3 (int a, int b)
{
  g = a / -b;
  return a / b;
}

__attribute__ ((noipa)) int
f4 (int a, int b)
{
  g = a / b;
  return a / -b;
}

int
main ()
{
  int m = -__INT_MAX__ - 1;
  if (f1 (m) != m / 2 || g != m / 2)
    __builtin_abort ();
  if (f2 (m) != m / 2 || g != m / 2)
    __builtin_abort ();
  if (f3 (m, m) != 1 || g != 1)
    __builtin_abort ();
  if (f4 (m, m) != 1 || g != 1)
    __builtin_abort ();
  return 0;
}
